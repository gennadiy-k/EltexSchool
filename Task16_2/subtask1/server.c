#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include "socket_in.h"

volatile sig_atomic_t flag = 1;

void sig_handler(int sig_num) {
    flag = 0; 
}

int main() {
    // Обработчик для SIGINT
    struct sigaction handler;
    memset(&handler, 0, sizeof(handler)); 
    sigemptyset(&handler.sa_mask);
    handler.sa_handler = sig_handler; 
    sigaction(SIGINT, &handler, NULL);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket (server)");
        exit(EXIT_FAILURE); 
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind (server)");
        exit(EXIT_FAILURE); 
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен\n");

    while (flag) { 
        int fd = accept(server_fd, NULL, NULL);
        if (fd < 0) {
            if (errno == EINTR) {
                if (!flag) {
                    break;
                }
                continue;
            }
            perror("accept");
            continue;
        }

        printf("Сервер: подключен клиент (сокет %d)\n", fd);

        struct ThreadData *data = malloc(sizeof(struct ThreadData));
        if (data == NULL) {
            perror("malloc");
            close(fd);
            continue;
        }

        data->client_fd = fd;

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, service_thread, (void*)data) != 0) {
            perror("pthread_create");
            free(data);
            close(fd);
            continue;
        }
    }

    printf("\nПолучен сигнал завершения работы сервера\n");
    close(server_fd);
    printf("Завершение работы\n");
    return 0;
}
