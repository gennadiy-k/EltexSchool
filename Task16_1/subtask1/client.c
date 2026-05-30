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

    char msg[] = "Hello!";
    char buf[BUF_SIZE];
    struct sockaddr_un serv_addr, cli_addr;

    int fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&cli_addr, 0, sizeof(cli_addr));
    cli_addr.sun_family = AF_LOCAL;
    strncpy(cli_addr.sun_path, CLI_PATH, sizeof(cli_addr.sun_path) - 1);

    if (bind(fd, (struct sockaddr*) &cli_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind client");
        exit(EXIT_FAILURE);
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_LOCAL;
    strncpy(serv_addr.sun_path, SERV_PATH, sizeof(SERV_PATH) - 1);

    if (connect(fd, (struct sockaddr*) &serv_addr, sizeof(struct sockaddr_un)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    
    if (send(fd, msg, sizeof(msg), 0) == -1) {
        perror("send"); 
        exit(EXIT_FAILURE);
    }
    printf("[КЛИЕНТ] Отправлено сообщение: %s\n", msg);

    if (recv(fd, buf, sizeof(buf) - 1, 0) == -1) { 
        perror("recv"); 
        exit(EXIT_FAILURE);
    }
    printf("[КЛИЕНТ] Получено сообщение: %s\n", buf);

    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    if (unlink(CLI_PATH) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    return 0;
}
