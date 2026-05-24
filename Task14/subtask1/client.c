#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/ipc.h>   
#include <sys/sem.h> 
#include <sys/shm.h>
#include <errno.h>
#include "message.h"

int main() {
    
    key_t key;
    
    int shmid, semid;
    char msg[MSG_SIZE] = "Hello!";
    char* addr;
    
    struct sembuf lock[2] = {{1, 0, 0}, {1, 1, 0}};
    struct sembuf unlock = {0, -1, 0};

    key = ftok("./server", 1);
    if (key == -1) {
        perror("Ошибка вызова ftok");
        exit(EXIT_FAILURE);
    }

    semid = semget(key, 2, 0);
    if (semid == -1) {
        perror("Ошибка вызова semget (server)");
        exit(EXIT_FAILURE);
    }

    shmid = shmget(key, 0, 0);
    if (shmid == -1) {
        perror("Ошибка вызова shmget");
        exit(EXIT_FAILURE);
    }

    addr = shmat(shmid, NULL, 0);
    if (addr == (void*) -1) {
        perror("Ошибка вызова shmat");
        exit(EXIT_FAILURE);
    }
    
    printf("Получено сообщение (клиент): %s\n", addr);
    semop(semid, lock, 2);

    memcpy(addr, msg, strlen(msg) + 1);
    printf("Отправлено сообщение (клиент): %s\n", addr);

    semop(semid, &unlock, 1);
    shmdt(addr);

    exit(EXIT_SUCCESS);
}