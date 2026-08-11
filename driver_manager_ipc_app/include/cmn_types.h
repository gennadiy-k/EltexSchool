#ifndef CMN_TYPES_H
#define CMN_TYPES_H

#include <sys/types.h> 

#define MAX_DRIVERS 100 // Максимальное количество водителей
#define BUFFER_SIZE 150

// Перечисление типов команд и статусов
typedef enum {
    CMD_SEND_TASK,      // Запрос на выдачу водителю задачи
    CMD_GET_STATUS,     // Запрос статуса
    CMD_RESP_AVAILABLE, // Статус: свободен (или задача принята)
    CMD_RESP_BUSY       // Статус: занят
} ipc_cmd_t;

// Структура сообщения
typedef struct {
    ipc_cmd_t cmd;      // Тип команды или статуса
    int timer_val;      // Значение таймера
} ipc_msg_t;

// Структура для хранения информации о запущенном процессе-водителе
typedef struct {
    pid_t pid;
    int socket_fd;
} driver_info_t;

#endif
