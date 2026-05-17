#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>         
#include <sys/stat.h>        
#include <mqueue.h>
#include "message.h"


int main() {

    mqd_t mqd_to_cli, mqd_to_srv;
    char message[MSG_SIZE] = "Hello";
    char client_buf[MAX_MSG_SIZE];

    mqd_to_cli = mq_open(QUEUE_TO_CLI, O_RDONLY);
    mqd_to_srv = mq_open(QUEUE_TO_SRV, O_WRONLY);

    if(mqd_to_cli == (mqd_t)-1 || mqd_to_srv == (mqd_t)-1) {
        perror("Ошибка mq_open (client)");
        exit(EXIT_FAILURE);
    }

    if (mq_receive(mqd_to_cli, client_buf, sizeof(client_buf), NULL) == -1) {
        perror("Ошибка mq_reveive (client)");
        exit(EXIT_FAILURE);
    }

    printf("Получено сообщение (клиент): %s\n", client_buf);

    if (mq_send(mqd_to_srv, "Hello", 6, 2) == -1) {
        perror("Ошибка mq_send (client)");
        exit(EXIT_FAILURE);
    }
    printf("Отправлено сообщение (клиент): %s\n", "Hello");

    mq_close(mqd_to_cli);
    mq_close(mqd_to_srv);
    exit(EXIT_SUCCESS);
}