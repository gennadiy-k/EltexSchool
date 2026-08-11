#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <poll.h>
#include "cmn_types.h"
#include "driver.h"
#include "cli_manager.h"

int main() {
    printf("Диспетчер такси запущен.\n");
    printf("Доступные команды:\n");
    printf("create_driver                       - создать нового водителя\n");
    printf("send_task <pid> <время в секундах>  - назначить задачу водителю\n");
    printf("get_status <pid>                    - получить статус водителя\n");
    printf("get_drivers                         - показать статус всех водителей\n");
    printf("Введите команду:\n");

    while (1) {
        printf(">: ");
        fflush(stdout);

        // Настройка poll на отслеживания ввода с клавиатуры
        struct pollfd pfd;
        pfd.fd = STDIN_FILENO; // Привязываем дескриптор стандартного потока ввода STDIN_FILENO
        pfd.events = POLLIN; // Событие появления данных для чтения

        // Блокируемся здесь до момента ввода данных
        poll(&pfd, 1, -1);

        // Читаем строку из консоли в буфер
        char buffer[BUFFER_SIZE];
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            break; // Если ввод прерван 
        }

        char cmd[64] = {0}; // Строковый массив для команды
        int arg_pid = 0;
        int arg_timer = 0;
        
        // Парсинг строки
        int args_cnt = sscanf(buffer, "%63s %d %d", cmd, &arg_pid, &arg_timer);

        if (args_cnt <= 0) {
            continue;
        }

        // Создаем нового водителя
        if (strcmp(cmd, "create_driver") == 0) {
            if (get_driver_count() >= MAX_DRIVERS) {
                printf("Достигнуто максимальное число активных водителей.\n");
                continue;
            }
            
            int sock[2];
            if (socketpair(AF_UNIX, SOCK_STREAM, 0, sock) < 0) {
                perror("socketpair");
                continue;
            }

            // Создаем дочерний процесс
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
                close(sock[0]);
                close(sock[1]);
            } 
            else if (pid == 0) {
                // Дочерний процесс (водителя)
                close(sock[0]); // Закрываем дескриптор
                run_driver(sock[1]);
            } 
            else {
                // Родительский процесс
                close(sock[1]);
                add_driver(pid, sock[0]); // Сохраняем PID водителя и дескриптор
                printf("Водитель создан с PID: %d\n", pid);
            }
        } 
        // Назначаем задачу водителю
        else if (strcmp(cmd, "send_task") == 0) {
            if (args_cnt < 3) {
                printf("Использование: send_task <pid> <время_задачи>\n");
                continue;
            }
            int idx = find_driver_idx(arg_pid); // Ищем индекс дескриптор по PID
            if (idx == -1) {
                printf("Водитель %d не найден\n", arg_pid);
            } 
            else {
                ipc_msg_t req;
                req.cmd = CMD_SEND_TASK; // Формируем команду на выдачу задачи
                req.timer_val = arg_timer;
                send_request_driver(idx, req); // Отправляем и выводим ответ
            }
        } 
        // Получаем статус водителя
        else if (strcmp(cmd, "get_status") == 0) {
            if (args_cnt < 2) {
                printf("Использование: get_status <pid>\n");
                continue;
            }
            int idx = find_driver_idx(arg_pid);
            if (idx == -1) {
                printf("Водитель %d не найден\n", arg_pid);
            } 
            else {
                ipc_msg_t req;
                req.cmd = CMD_GET_STATUS; // Формируем команду на запрос статуса
                req.timer_val = 0;
                send_request_driver(idx, req);
            }
        } 
        
        // Получаем статус всех водителей
        else if (strcmp(cmd, "get_drivers") == 0) {
            int ttl_drivers = get_driver_count(); // Запрашиваем общее количество водителей

            if (ttl_drivers == 0) {
                printf("Нет активных водителей\n");
                continue;
            }

            // Опрос всех водителей
            for (int driver_index = 0; driver_index < ttl_drivers; driver_index++) {
                ipc_msg_t req;
                req.cmd = CMD_GET_STATUS; // Формируем команду запрос статуса
                req.timer_val = 0;
                send_request_driver(driver_index, req); // Вызываем отправку запроса по индексу водителя
            }
        }
        // Ошибка ввода
        else {
            printf("Неизвестная команда\n");
        }
    }
    return 0;
}
