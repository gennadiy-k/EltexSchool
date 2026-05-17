#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/ipc.h>    
#include <sys/msg.h> 
#include "message.h"

int main() {
    
    key_t key;
    int msqid;

    Message data = {.mtype = 1,.mtext = "Hi!"};
    
    key = ftok("./server", 1);
    if (key == -1) {
        perror("Ошибка вызова ftok");
        exit(EXIT_FAILURE);
    }

    msqid = msgget(key, IPC_CREAT | 0666);
    if (msqid == -1) {
        perror("Ошибка вызова msgget");
        exit(EXIT_FAILURE);
    }

    if (msgsnd(msqid, &data, sizeof(data.mtext), 0) == -1) {
        perror("Ошибка вызова msgsnd");
        exit(EXIT_FAILURE);
    }

    printf("Сообщение отправлено (сервер): %s\n", data.mtext);

    if (msgrcv(msqid, &data, sizeof(data.mtext), 2, 0) == -1) {
        perror("Ошибка вызова msgrcv");
        exit(EXIT_FAILURE);
    }

    printf("Получено сообщение (сервер): %s\n", data.mtext);
    msgctl(msqid, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}