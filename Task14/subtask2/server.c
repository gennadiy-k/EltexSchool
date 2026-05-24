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
    
    char msg[MSG_SIZE] = "Hi!";
    int fd;
    char* shmp;

    sem_t* sem_server;
    sem_t* sem_client;

    fd = shm_open(SHM_SERVER, O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("Ошибка shm_open (server)");
        exit(EXIT_FAILURE);
    }

    sem_server = sem_open(SEM_SERVER, O_CREAT, 0666, 0);
    sem_client = sem_open(SEM_CLIENT, O_CREAT, 0666, 0);
    if (sem_server == SEM_FAILED || sem_client == SEM_FAILED) {
        perror("Ошибка sem_open (server)");
        exit(EXIT_FAILURE);       
    }

    if (ftruncate(fd, MEM_SIZE) == -1) {
        perror("Ошибка ftruncate (server)");
        exit(EXIT_FAILURE);
    }

    shmp = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        perror("Ошибка mmap (server)");
        exit(EXIT_FAILURE);        
    }

    close(fd);

    memcpy(shmp, msg, strlen(msg) + 1);
    printf("Отправлено сообщение (сервер): %s\n", shmp);

    sem_post(sem_server);

    sem_wait(sem_client);
    
    printf("Получено сообщение (сервер): %s\n", shmp);

    munmap(shmp, MEM_SIZE);
    shm_unlink(SHM_SERVER);
    sem_close(sem_server); 
    sem_close(sem_client); 
    sem_unlink(SEM_SERVER);
    sem_unlink(SEM_CLIENT);
    
    exit(EXIT_SUCCESS);
}

