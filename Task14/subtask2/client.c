#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/mman.h>    
#include <fcntl.h>
#include <sys/stat.h> 
#include <semaphore.h>
#include "message.h"

int main() {

    char msg[MSG_SIZE] = "Hello!";
    int fd;
    char* shmp;
    
    sem_t* sem_server;
    sem_t* sem_client;

    fd = shm_open(SHM_SERVER, O_RDWR, 0666);
    if (fd == -1) {
        perror("Ошибка shm_open (server)");
        exit(EXIT_FAILURE);
    }

    sem_server = sem_open(SEM_SERVER, 0);
    sem_client = sem_open(SEM_CLIENT, 0);
    if (sem_server == SEM_FAILED || sem_client == SEM_FAILED) {
        perror("Ошибка sem_open (server)");
        exit(EXIT_FAILURE);       
    }

    shmp = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        perror("Ошибка mmap (client)");
        exit(EXIT_FAILURE);        
    }
    
    sem_wait(sem_server);
    printf("Получено сообщение (клиент): %s\n", shmp);
    
    memcpy(shmp, msg, strlen(msg) + 1);
    printf("Отправлено сообщение (клиент): %s\n", shmp);

    sem_post(sem_client);

    exit(EXIT_SUCCESS);
}