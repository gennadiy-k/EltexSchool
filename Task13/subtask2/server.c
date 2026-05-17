#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>         
#include <sys/stat.h>        
#include <mqueue.h>
#include "message.h"


int main() {
    
    char message[MSG_SIZE] = "Hi!";
    char server_buf[MAX_MSG_SIZE];
    mqd_t mqd_to_cli, mqd_to_srv;

    mq_unlink(QUEUE_TO_CLI); 
    mq_unlink(QUEUE_TO_SRV); 

    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = MAX_MSG, // Максимум 10 сообщений в очереди 
        .mq_msgsize = MAX_MSG_SIZE, // Максимальный размер 1 сообщения (в байтах)
        .mq_curmsgs = 0, //Текущее кол-во сообщений
    };

    mqd_to_cli = mq_open(QUEUE_TO_CLI, O_CREAT | O_WRONLY, 0666, &attr);
    mqd_to_srv = mq_open(QUEUE_TO_SRV, O_CREAT | O_RDONLY, 0666, &attr);

    if(mqd_to_cli == (mqd_t)-1 || mqd_to_srv == (mqd_t)-1) {
        perror("Ошибка mq_open (server)");
        exit(EXIT_FAILURE);
    }

    if (mq_send(mqd_to_cli, message, sizeof(message), 1) == -1) {
        perror("Ошибка mq_send (server)");
        exit(EXIT_FAILURE);
    }

    printf("Отправлено сообщение (сервер): %s\n", message);

    if (mq_receive(mqd_to_srv, server_buf, sizeof(server_buf), NULL) == -1) {
        perror("Ошибка mq_reveive (server)");
        exit(EXIT_FAILURE);
    }

    printf("Получено сообщение (сервер): %s\n", server_buf);

    mq_close(mqd_to_cli);
    mq_close(mqd_to_srv);
    mq_unlink(QUEUE_TO_CLI);
    mq_unlink(QUEUE_TO_SRV);
    exit(EXIT_SUCCESS);
}