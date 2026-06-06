#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "socket_in.h"

void* client_thread(void* arg) {
    int thread_id = *(int*)arg;
    struct sockaddr_in serv_addr;
    char time_buf[32] = {0};

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return NULL;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP_ADDR, &serv_addr.sin_addr);

    // Подключаемся к серверу
    if (connect(fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        close(fd);
        return NULL;
    }

    // Ждем ответ от сервера
    int nread = recv(fd, time_buf, sizeof(time_buf) - 1, 0);
    if (nread > 0) {
        time_buf[nread] = '\0';
        time_buf[strcspn(time_buf, "\n")] = 0; 
        printf("Поток %d | Время: %s\n", thread_id, time_buf);
    } 
    else {
        printf("Поток %d | Ошибка получения данных\n", thread_id);
    }

    close(fd);
    return NULL;
}

int main() {
    pthread_t threads[CLIENTS_NUM];
    int thread_ids[CLIENTS_NUM];

    for (int i = 0; i < CLIENTS_NUM; i++) {
        thread_ids[i] = i;
        if (pthread_create(&threads[i], NULL, client_thread, &thread_ids[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < CLIENTS_NUM; i++) {
        pthread_join(threads[i], NULL);
    }
    
    return 0;
}