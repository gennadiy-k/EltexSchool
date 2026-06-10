#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "socket_in.h"

int main() {

    char buf[BUF_SIZE];
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd, (struct sockaddr*) &serv_addr, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    cli_len = sizeof(struct sockaddr_in);

    while (1) {
        memset(buf, 0, BUF_SIZE);
        ssize_t nread = recvfrom(fd, buf, sizeof(buf) - 1, 0, (struct sockaddr*) &cli_addr, &cli_len);
        if (nread == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        buf[nread] = '\0';
        printf("[СЕРВЕР] Получено сообщение: %s\n", buf);

        // Модификация строки от клиента
        snprintf(&buf[nread], BUF_SIZE - (nread - 1), "FGH");
        int new_len = strlen(buf); 

        if (sendto(fd, buf, new_len, 0, (struct sockaddr*) &cli_addr, cli_len) == -1) {
            perror("sendto"); 
            exit(EXIT_FAILURE);
        }
        printf("[СЕРВЕР] Отправлено сообщение клиенту: %s\n", buf);
    }
    
    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    return 0;
}