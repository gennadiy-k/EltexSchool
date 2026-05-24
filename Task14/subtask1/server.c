#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/ipc.h>    
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/sem.h> 
#include "message.h"


int main() {
    
    key_t key;

    int shmid, semid;
    char msg[MSG_SIZE] = "Hi!";
    char *addr;
    
    struct sembuf lock[2] = {{0, 0, 0}, {0, 1, 0}};
    struct sembuf unlock = {1, -1, 0};
    
    key = ftok("./server", 1);
    if (key == -1) {
        perror("Ошибка вызова ftok (server)");
        exit(EXIT_FAILURE);
    }

    semid = semget(key, 2, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("Ошибка вызова semget (server)");
        exit(EXIT_FAILURE);
    }

    shmid = shmget(key, MEM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("Ошибка вызова shmget (server)");
        exit(EXIT_FAILURE);
    }

    addr = shmat(shmid, NULL, 0);
    if (addr == (void*) -1) {
        perror("Ошибка вызова shmat (server)");
        exit(EXIT_FAILURE);
    }

    memcpy(addr, msg, strlen(msg) + 1);
    printf("Сообщение отправлено (сервер): %s\n", addr);
    
    semop(semid, lock, 2);
    semop(semid, &unlock, 1);

    printf("Получено сообщение (сервер): %s\n", addr);

    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    
    exit(EXIT_SUCCESS);
}