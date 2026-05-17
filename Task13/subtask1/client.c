#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/ipc.h>    
#include <sys/msg.h> 
#include <errno.h>
#include "message.h"

int main() {
    
    key_t key;
    int msqid;

    Message data;

    key = ftok("./server", 1);
    if (key == -1) {
        perror("Ошибка вызова ftok");
        exit(EXIT_FAILURE);
    }

    msqid = msgget(key, 0);
    if (msqid == -1) {
        perror("Ошибка вызова msgget");
        exit(EXIT_FAILURE);
    }

    if (msgrcv(msqid, &data, sizeof(data.mtext), 1, 0) == -1) {
        perror("Ошибка вызова msgrcv");
        exit(EXIT_FAILURE);
    } 

    printf("Получено сообщение (клиент): %s\n", data.mtext);
    snprintf(data.mtext, sizeof(data.mtext), "Hello");
    data.mtype = 2;

    if (msgsnd(msqid, &data, sizeof(data.mtext), 0) == -1) {
        perror("Ошибка вызова msgsnd");
        exit(EXIT_FAILURE);
    }

    printf("Отправлено сообщение (клиент): %s\n", data.mtext);

    exit(EXIT_SUCCESS);
}