#ifndef SOCKET_IN_H
#define SOCKET_IN_H

#include <pthread.h>
#include <signal.h>

#define PORT 8080
#define IP_ADDR "127.0.0.1"

#define POOL_SIZE 4                 // Размер пула потоков обслуживания
#define CLIENTS_NUM 10              // Количество клиентов на подключение (потоков)

struct ThreadPool;

// Структура данных для пула потоков
struct ThreadData {
    struct ThreadPool *pool;
    int id;                         // ID потока обслуживания
};

// Структура данных для сервера
struct ThreadPool {
    int busy[POOL_SIZE];            // Флаги занятости потоков (1 - занят, 0 - свободен)
    int sockets[POOL_SIZE];         // Дескрипторы сокетов клиентов
    int pool_ids[POOL_SIZE];        // ID потоков обслуживания
    pthread_cond_t cond_service;    // Блокировка потоков при отсутствии клиентов
    pthread_cond_t cond_main;       // Условная переменная для блокировки слушающего сервера при отсутствии свободных потоков
    pthread_mutex_t mutex;   
    volatile sig_atomic_t flag;                
    
    struct ThreadData threads_data[POOL_SIZE];  // Массив структур для потоков
};

// Прототип функции потока
void* service_thread(void* arg);

#endif
