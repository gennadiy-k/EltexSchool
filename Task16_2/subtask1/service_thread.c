#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <pthread.h>
#include "socket_in.h"

void* service_thread(void* arg) {
    // Отсоединяем поток
    pthread_detach(pthread_self());

    struct ThreadData* data = (struct ThreadData*)arg;
    int fd = data->client_fd;
    free(data); 

    // Отправляем время
    time_t cur_time = time(NULL);
    char time_buf[32];
    ctime_r(&cur_time, time_buf); 

    send(fd, time_buf, strlen(time_buf), 0);
    
    close(fd);
    printf("Клиент успешно обслужен (сокет %d)\n", fd);
    
    return NULL;
}
