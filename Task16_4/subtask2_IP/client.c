#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include "socket_in.h"

int main() {

    char buf[BUF_SIZE];
    struct sockaddr_in serv_addr;
    
    struct iphdr* ip = (struct iphdr*) buf;
    struct udphdr* udp = (struct udphdr*) (buf + sizeof(struct iphdr));
    char* payload = buf + sizeof(struct iphdr) + sizeof(struct udphdr); // Начало данных после заголовков IP и UDP

    memcpy(payload, "ABCD", 5); // Копируем строку
    int payload_len = strlen(payload);
    int udp_packet_len = sizeof(struct udphdr) + payload_len;
    int full_packet_len = sizeof(struct iphdr) + udp_packet_len;

    // Заполняем UDP заголовок (транспортный уровень)
    udp->uh_sport = htons(CLIENT_PORT); // Порт отправителя (клиентский)
    udp->uh_dport = htons(SERVER_PORT); // Порт получателя (сервера)
    udp->uh_ulen = htons(udp_packet_len); // Длина пакета
    udp->uh_sum = 0; // Чексумма

    // Заполняем IP заголовок (сетевой уровень)
    ip->version = 4; 
    ip->ihl = 5; // Длина заголовка
    ip->tos = 0;
    ip->tot_len = 0; // Общая длина пакета
    ip->id = 0; // Идентификатор пакета                  
    ip->frag_off = 0; // Без фрагментации
    ip->ttl = 10; // Время жизни пакета
    ip->protocol = IPPROTO_UDP; // Протокол транспортного уровня
    ip->check = 0; // Чексумма                    

    // Заполняем IP адреса
    if (inet_pton(AF_INET, IP_ADDR, &ip->saddr) <= 0 || inet_pton(AF_INET, IP_ADDR, &ip->daddr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    int opt = 1;
    if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, IP_ADDR, &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }
    
    if (sendto(fd, buf, full_packet_len, 0, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
        perror("sendto"); 
        exit(EXIT_FAILURE);
    }
    printf("[КЛИЕНТ] Серверу отправлен пакет со строкой %s\n", payload);

    char recv_buf[BUF_SIZE];
    char* data;
    int data_len;
    int header_size = sizeof(struct iphdr) + sizeof(struct udphdr); // 28 байт (IP и UDP заголовки)

    while (1) {
        memset(recv_buf, 0, sizeof(recv_buf));
        ssize_t nread = recvfrom(fd, recv_buf, sizeof(recv_buf), 0, NULL, NULL);
        if (nread == -1) { 
            perror("recvfrom"); 
            exit(EXIT_FAILURE);
        }

        if (nread >= header_size) {
            struct udphdr* recv_udp = (struct udphdr*)(recv_buf + sizeof(struct iphdr));

            // Проверяем, равен ли порт получателя пакета клиенсткому порту
            if (ntohs(recv_udp->uh_dport) == CLIENT_PORT) {
                data = recv_buf + header_size;
                data_len = nread - header_size;
                
                if (data_len >= 0) {
                    data[data_len] = '\0'; 
                }
                break; 
            }
        }
    }

    printf("[КЛИЕНТ] Получено сообщение: %s\n", data);

    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    return 0;
}
