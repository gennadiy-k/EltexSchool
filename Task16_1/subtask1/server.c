#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "socket_un.h"

int main() {

    char buf[BUF_SIZE];
    char msg[] = "Hi!";
    socklen_t cli_len;
    struct sockaddr_un serv_addr, cli_addr;

    int fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    unlink(SERV_PATH);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_LOCAL;
    strncpy(serv_addr.sun_path, SERV_PATH, sizeof(SERV_PATH) - 1);

    if (bind(fd, (struct sockaddr*) &serv_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    cli_len = sizeof(struct sockaddr_un);

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

    if (unlink(SERV_PATH) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    return 0;
}