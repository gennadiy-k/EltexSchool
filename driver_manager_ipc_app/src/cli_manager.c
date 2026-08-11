#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "cli_manager.h"

static driver_info_t drivers[MAX_DRIVERS]; // Массив структур для хранения информации о водителях
static int driver_cnt = 0;

// Функция добавления нового водителя в массив структур
void add_driver(pid_t pid, int fd) {
    if (driver_cnt < MAX_DRIVERS) {
        drivers[driver_cnt].pid = pid; // Сохраняем PID процесса-водителя
        drivers[driver_cnt].socket_fd = fd; // Сохраняем дескриптор
        driver_cnt++; // Увеличиваем счетчик водителей
    }
}

// Функция получения текущего количества водителей
int get_driver_count(void) {
    return driver_cnt;
}

// Функция поиска индекса водителя в массиве структур по его PID
int find_driver_idx(pid_t pid) {
    for (int i = 0; i < driver_cnt; i++) {
        if (drivers[i].pid == pid) {
            return i;
        }
    }
    return -1;
}

// Функция отправки запроса конкретному водителю и ожидания ответа
void send_request_driver(int idx, ipc_msg_t req) {
    // Отправляем структуру запроса в сокет выбранного водителя
    write(drivers[idx].socket_fd, &req, sizeof(req));

    // Настраиваем poll для ожидания ответа от водителя
    struct pollfd pfd;
    pfd.fd = drivers[idx].socket_fd;
    pfd.events = POLLIN; // Событие появления данных для чтения

    // Блокируемся максимум на 1000 мс в ожидании ответа
    if (poll(&pfd, 1, 1000) > 0) {
        ipc_msg_t resp; // Структура для хранения ответа
        
        // Читаем пришедший от выбранного водителя ответ
        if (read(drivers[idx].socket_fd, &resp, sizeof(resp)) > 0) {
            switch (resp.cmd) {
                case CMD_RESP_AVAILABLE: // Водитель свободен
                    // Если назначали ему задачу, подтверждаем что она запустилась
                    if (req.cmd == CMD_SEND_TASK) {
                        printf("Водитель %d: задача успешно запущена\n", drivers[idx].pid);
                    } 
                    // Если это запрос статуса, выводим что водитель свободен
                    else {
                        printf("Водитель %d: свободен\n", drivers[idx].pid);
                    }
                    break;

                case CMD_RESP_BUSY: // Водитель занят
                    // Выводим статус и количество оставшихся секунд из ответа
                    printf("Водитель %d: занят %d сек\n", drivers[idx].pid, resp.timer_val);
                    break;

                default:
                    break;
            }
            return;
        }
    }
    // Выполняем, если poll вернул 0 (таймаут 1 сек) или это ошибка read
    printf("Водитель %d: нет ответа (процесс отключен)\n", drivers[idx].pid);
}

