#define _DEFAULT_SOURCE
#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <mqueue.h>
#include "socket_in.h"

void* service_thread(void* arg) {
    struct ThreadData* data = (struct ThreadData*)arg;
    struct ThreadPool* pool = data->pool;
    int id = data->id;

    printf("Поток %d запущен\n", id);

    while (pool->flag) {
        int client_fd;
        
        ssize_t bytes_read = mq_receive(pool->mq, (char*)&client_fd, sizeof(int), NULL);
        
        if (bytes_read == -1) {
            if (!pool->flag) break;
            perror("mq_receive");
            continue;
        }

        printf("Поток %d обслуживает клиента (сокет %d)\n", id, client_fd);

        // Отправляем время клиенту
        time_t cur_time = time(NULL);
        char time_buf[32];
        ctime_r(&cur_time, time_buf);
        
        send(client_fd, time_buf, strlen(time_buf), 0);
        close(client_fd);

        printf("Поток %d освободился\n", id);
    }
    return NULL;
}
