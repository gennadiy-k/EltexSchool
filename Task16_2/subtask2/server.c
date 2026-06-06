#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include "socket_in.h"

struct ThreadPool server_pool = {0};

void sig_handler(int sig_num) {
    server_pool.flag = 0; 
}

int main() {
    // Обработчик для SIGINT
    struct sigaction handler;
    memset(&handler, 0, sizeof(handler)); 
    sigemptyset(&handler.sa_mask);
    handler.sa_handler = sig_handler; 
    sigaction(SIGINT, &handler, NULL);

    pthread_mutex_init(&server_pool.mutex, NULL);
    pthread_cond_init(&server_pool.cond_service, NULL);
    pthread_cond_init(&server_pool.cond_main, NULL);
    server_pool.flag = 1;

    // Создаем пул обслуживающих потоков
    pthread_t pool[POOL_SIZE];
    for (int i = 0; i < POOL_SIZE; i++) {
        server_pool.pool_ids[i] = i;
        server_pool.busy[i] = 0;

        server_pool.threads_data[i].pool = &server_pool;
        server_pool.threads_data[i].id = i;

        // Передаем указатель на структуру данных конкретного потока
        if (pthread_create(&pool[i], NULL, service_thread, &server_pool.threads_data[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен\n");

    while (server_pool.flag) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            if (errno == EINTR) {
                if (!server_pool.flag) {
                    break;
                }
                continue;
            }
            perror("accept");
            continue;
        }
        printf("Cервер: клиент подключен (сокет %d)\n", client_fd);

        pthread_mutex_lock(&server_pool.mutex);

        int free_index = -1;
        
        // Ожидание свободного потока (если все заняты, блокируемся и ждем уведомления)
        while (server_pool.flag) {
            for (int i = 0; i < POOL_SIZE; i++) {
                if (server_pool.busy[i] == 0) {
                    free_index = i; // Первый попавшийся свободный поток
                    break;
                }
            }
            
            // Если поток найден или сервер выключается, то выходим из цикла ожидания
            if (free_index != -1 || !server_pool.flag) {
                break; 
            }
            
            printf("Сервер: все потоки заняты\n");
            pthread_cond_wait(&server_pool.cond_main, &server_pool.mutex);
        }

        if (!server_pool.flag) {
            close(client_fd);
            pthread_mutex_unlock(&server_pool.mutex);
            break;
        }
        
        server_pool.sockets[free_index] = client_fd;
        server_pool.busy[free_index] = 1; // Ставим флаг занятости
        
        pthread_cond_broadcast(&server_pool.cond_service);
        pthread_mutex_unlock(&server_pool.mutex);
    }

    printf("\nПолучен сигнал завершения работы сервера\n");
    
    pthread_mutex_lock(&server_pool.mutex);
    server_pool.flag = 0;
    pthread_cond_broadcast(&server_pool.cond_service); 
    pthread_mutex_unlock(&server_pool.mutex);

    for (int i = 0; i < POOL_SIZE; i++) {
        pthread_join(pool[i], NULL);
    }
    
    pthread_mutex_destroy(&server_pool.mutex);
    pthread_cond_destroy(&server_pool.cond_service);
    pthread_cond_destroy(&server_pool.cond_main);
    close(server_fd);
    printf("Завершение работы\n");
    return 0;
}
