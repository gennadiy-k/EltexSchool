#ifndef SOCKET_IN_H
#define SOCKET_IN_H


#define PORT 8080
#define IP_ADDR "127.0.0.1"
#define CLIENTS_NUM 10      // Количество клиентов на подключение (потоков)

// Структура для передачи данных в поток
struct ThreadData {
    int client_fd;
};

// Прототип функции потока
void* service_thread(void* arg);

#endif
