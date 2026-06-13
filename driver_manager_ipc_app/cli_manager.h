#ifndef CLI_MANAGER_H
#define CLI_MANAGER_H

#include "cmn_types.h"

// Прототипы функций для управления процессами водителей
// Добавление нового водителя в массив структур
void add_driver(pid_t pid, int fd);

// Поиск индекса водителя в массиве структур по его PID
int  find_driver_idx(pid_t pid);

// Отправка запроса конкретному водителю и ожидания ответа 
void send_request_driver(int idx, ipc_msg_t req);

// Получение текущего количества водителей
int  get_driver_count(void);

#endif
