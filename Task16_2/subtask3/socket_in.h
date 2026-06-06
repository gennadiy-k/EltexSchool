#ifndef SOCKET_IN_H
#define SOCKET_IN_H

#include <pthread.h>
#include <mqueue.h>
#include <signal.h>

#define PORT 8080
#define IP_ADDR "127.0.0.1"

#define CLIENTS_NUM 10          // Количество клиентов на подключение (потоков)
#define POOL_SIZE 4             // Размер пула потоков обслуживания
#define MQ_SIZE 10              // Размер очереди
#define MQ_SERVER "/time_server_queue"

struct ThreadPool;

// Структура данных для пула потоков
struct ThreadData {
    struct ThreadPool *pool;
    int id;                     // ID потока обслуживания
};

// Структура данных для сервера
struct ThreadPool {
    mqd_t mq;
    int pool_ids[POOL_SIZE];    // ID потоков обслуживания 
    volatile sig_atomic_t flag;                 
    
    struct ThreadData threads_data[POOL_SIZE];  // Массив структур для потоков
};

// Прототип функции потока
void* service_thread(void* arg);

#endif
