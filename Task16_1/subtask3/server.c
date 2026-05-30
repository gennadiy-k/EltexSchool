#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "socket_in.h"

int main() {

    char buf[BUF_SIZE];
    char msg[] = "Hi!";
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_NUM);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd, (struct sockaddr*) &serv_addr, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    cli_len = sizeof(struct sockaddr_in);
    if (recvfrom(fd, buf, sizeof(buf) - 1, 0, (struct sockaddr*) &cli_addr, &cli_len) == -1) {
        perror("recvfrom");
        exit(EXIT_FAILURE);
    }
    printf("[СЕРВЕР] Получено сообщение: %s\n", buf);

    if (sendto(fd, msg, sizeof(msg), 0, (struct sockaddr*) &cli_addr, cli_len) == -1) {
        perror("sendto"); 
        exit(EXIT_FAILURE);
    }
    printf("[СЕРВЕР] Отправлено сообщение: %s\n", msg);
    
    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    return 0;
}