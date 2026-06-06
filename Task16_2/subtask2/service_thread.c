#define _DEFAULT_SOURCE
#include <stdio.h>
#include <time.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include "socket_in.h"

void* service_thread(void* arg) {
    struct ThreadData* data = (struct ThreadData*)arg;
    struct ThreadPool* pool = data->pool;

    int id = data->id;

    printf("Поток %d запущен\n", id);

    while (pool->flag) {
        pthread_mutex_lock(&pool->mutex);

        // Поток ожидает установки флага занятости
        while (pool->busy[id] == 0 && pool->flag) {
            pthread_cond_wait(&pool->cond_service, &pool->mutex);
        }

        if (!pool->flag) {
            pthread_mutex_unlock(&pool->mutex);
            break;
        }

        // Забираем сокет из массива
        int client_fd = pool->sockets[id];
        pthread_mutex_unlock(&pool->mutex);

        printf("Поток %d обслуживает клиента (сокет %d)\n", id, client_fd);

        // Отправляем время клиенту
        time_t cur_time = time(NULL);
        char time_buf[32];
        ctime_r(&cur_time, time_buf);
        send(client_fd, time_buf, strlen(time_buf), 0);
        close(client_fd);

        // Уведомляем слушающий сервер
        pthread_mutex_lock(&pool->mutex);
        pool->busy[id] = 0; // Сбрасываем флаг занятости
        pthread_cond_signal(&pool->cond_main); 
        pthread_mutex_unlock(&pool->mutex);

        printf("Поток %d освободился\n", id);
    }
    return NULL;
}
