#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "socket_in.h"

int main() {
    int fd;
    struct sockaddr_in addr, cli_addr;
    char buf[BUF_SIZE];

    socklen_t addr_len = sizeof(cli_addr);
    
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT_NUM);
    if (inet_pton(AF_INET, IP_ADDR, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 10; i++) {
        memset(buf, 0, BUF_SIZE);
        recvfrom(fd, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&cli_addr, &addr_len);
        printf("Клиент | Получено #%d: %s\n", i + 1, buf);
    }

    close(fd);
    return 0;
}
