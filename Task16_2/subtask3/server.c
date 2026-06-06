#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
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

    server_pool.flag = 1;

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MQ_SIZE;           
    attr.mq_msgsize = sizeof(int);         
    attr.mq_curmsgs = 0;

    mq_unlink(MQ_SERVER);

    server_pool.mq = mq_open(MQ_SERVER, O_CREAT | O_RDWR, 0666, &attr);
    if (server_pool.mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    pthread_t pool[POOL_SIZE];
    for (int i = 0; i < POOL_SIZE; i++) {
        server_pool.pool_ids[i] = i;
        server_pool.threads_data[i].pool = &server_pool;
        server_pool.threads_data[i].id = i;

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

    printf("Cервер запущен\n");

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

        // Отправляем дескриптор в очередь
        if (mq_send(server_pool.mq, (const char*)&client_fd, sizeof(int), 0) == -1) {
            if (!server_pool.flag) {
                close(client_fd);
                break;
            }
            perror("mq_send");
            close(client_fd);
        }
    }

    printf("\nПолучен сигнал завершения работы сервера\n");
    server_pool.flag = 0;

    mq_close(server_pool.mq);
    mq_unlink(MQ_SERVER);

    for (int i = 0; i < POOL_SIZE; i++) {
        pthread_cancel(pool[i]); 
        pthread_join(pool[i], NULL);
    }
    
    close(server_fd);
    printf("Завершение работы\n");
    return 0;
}
