#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>
#include "driver.h"
#include "cmn_types.h"

// Функция обработки входящих команд процесса-водителя
void run_driver(int sock) {
    int is_busy = 0; // Флаг занятости: 0 - свободен, 1 - занят заказом
    time_t task_end_time = 0; // Время, через которое задача завершится
    ipc_msg_t msg; // Структура для чтения входящих команд

    while (1) {
        // Если водитель занят и текущее время превысило время окончания задачи
        if (is_busy && time(NULL) >= task_end_time) {
            is_busy = 0;
        }

        // Настраиваем структуру poll для ожидания команд
        struct pollfd pfd;
        pfd.fd = sock;
        pfd.events = POLLIN; // Событие появления данных для чтения
        
        // Таймаут 500 мс для проверки таймера выше
        int ret = poll(&pfd, 1, 500);
        if (ret <= 0) {
            continue; // Уходим на новую итерацию для проверки времени
        }
        
        // Если в дескриптор пришли данные
        if (pfd.revents & POLLIN) {
            // Читаем команду из сокета
            int n = read(sock, &msg, sizeof(msg));
            if (n <= 0) {
                break; // Если read вернул 0 или меньше (ошибка/завершение родительского процесса)
            }

            // Повторная проверка перед ответом
            if (is_busy && time(NULL) >= task_end_time) {
                is_busy = 0;
            }

            ipc_msg_t reply; // Структура для отправки ответа

            switch (msg.cmd) {
                case CMD_SEND_TASK: // Пришла команда взять задачу
                    if (is_busy) {
                        // Если уже заняты, то сообщаем об этом диспетчеру и передаем сколько секунд осталось
                        reply.cmd = CMD_RESP_BUSY;
                        reply.timer_val = (int)(task_end_time - time(NULL));
                        write(sock, &reply, sizeof(reply));
                    } 
                    else {
                        // Если свободны, то сообщаем об этом диспетчеру и считаем время завершения задачи (сейчас + таймер)
                        is_busy = 1;
                        task_end_time = time(NULL) + msg.timer_val;
                        reply.cmd = CMD_RESP_AVAILABLE; 
                        reply.timer_val = 0;
                        write(sock, &reply, sizeof(reply));
                    }
                    break;

                case CMD_GET_STATUS: // Пришел запрос статуса
                    if (is_busy) {
                        // Если заняты, то сообщаем об этом диспетчеру и передаем сколько секунд осталось
                        reply.cmd = CMD_RESP_BUSY;
                        reply.timer_val = (int)(task_end_time - time(NULL));
                        write(sock, &reply, sizeof(reply));
                    } 
                    else {
                        // Если свободны, то сообщаем об этом диспетчеру
                        reply.cmd = CMD_RESP_AVAILABLE;
                        reply.timer_val = 0;
                        write(sock, &reply, sizeof(reply));
                    }
                    break;

                default:
                    break;
            }
        }
    }
    
    close(sock);
    exit(EXIT_SUCCESS);
}
