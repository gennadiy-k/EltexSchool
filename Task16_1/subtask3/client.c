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

    char msg[] = "Hello!";
    char buf[BUF_SIZE];
    struct sockaddr_in serv_addr;

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_NUM);
    if (inet_pton(AF_INET, IP_ADDR, &serv_addr.sin_addr) <= 0) {
        perror("inet_pton failed");
        exit(EXIT_FAILURE);
    }
    
    if (sendto(fd, msg, sizeof(msg), 0, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
        perror("sendto"); 
        exit(EXIT_FAILURE);
    }
    printf("[КЛИЕНТ] Отправлено сообщение: %s\n", msg);

    if (recvfrom(fd, buf, sizeof(buf), 0, NULL, NULL) == -1) { 
        perror("recvfrom"); 
        exit(EXIT_FAILURE);
    }
    printf("[КЛИЕНТ] Получено сообщение: %s\n", buf);

    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    return 0;
}
