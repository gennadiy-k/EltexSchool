#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include "socket_in.h"
#include "clients_list.h"

// Флаг для остановки главного цикла при получении SIGINT
volatile sig_atomic_t flag = 1;

void sig_handler(int sig) { 
    flag = 0; 
}

int main(void) {
    ClientList c_list;
    c_list.head = NULL; 

    // Обработчик для SIGINT
    struct sigaction handler;
    memset(&handler, 0, sizeof(handler)); 
    sigemptyset(&handler.sa_mask);
    handler.sa_handler = sig_handler; 
    sigaction(SIGINT, &handler, NULL);

    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (fd < 0) { 
        perror("socket"); 
        exit(EXIT_FAILURE); 
    }

    int opt = 1; 
    setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt));

    char buf[BUFFER_SIZE];
    printf("Сервер запущен. Порт: %d\n", SERVER_PORT);

    while (flag) {
        ssize_t res = recvfrom(fd, buf, BUFFER_SIZE, 0, NULL, NULL);
        if (res < 0) { 
            if (!flag) { 
                break;
            } 
            continue; 
        }

        // Обработка заголовков
        struct iphdr *ip = (struct iphdr*)buf;
        struct udphdr *udp = (struct udphdr*)(buf + sizeof(struct iphdr));
        if (ntohs(udp->dest) != SERVER_PORT) {
            continue; // Проверка порта назначения (если адресовано не на порт сервера)
        }

        // Извлечение данных
        char payload[BUFFER_SIZE];
        int payload_len = ntohs(udp->len) - sizeof(struct udphdr);
        memcpy(payload, buf + sizeof(struct iphdr) + sizeof(struct udphdr), payload_len);
        payload[payload_len] = '\0';

        struct in_addr print_addr;
        print_addr.s_addr = ip->saddr;

        // Обработка отключения клиента
        if (strcmp(payload, DISCONNECT) == 0) {
            printf("Клиент %s:%d отключился\n", inet_ntoa(print_addr), ntohs(udp->source));
            remove_client(&c_list, ip->saddr, udp->source);
            continue;
        }

        printf("Получено от %s:%d: %s\n", inet_ntoa(print_addr), ntohs(udp->source), payload);

        // Находим/создаем клиента и увеличиваем счетчик сообщений
        int count = reg_client_msg(&c_list, ip->saddr, udp->source);

        // Подготовка буфера для отправки
        char send_buf[BUFFER_SIZE];
        memset(send_buf, 0, BUFFER_SIZE);
        struct iphdr *ip_send = (struct iphdr *) send_buf;
        struct udphdr *udp_send = (struct udphdr *) (send_buf + sizeof(struct iphdr));
        int reply_len = snprintf(send_buf + sizeof(struct iphdr) + sizeof(struct udphdr), 
            BUFFER_SIZE - sizeof(struct iphdr) - sizeof(struct udphdr), "%s %d", payload, count);

        // Заполнение полей IP заголовка ответа
        ip_send->ihl = 5; 
        ip_send->version = 4; 
        ip_send->ttl = 10; 
        ip_send->protocol = IPPROTO_UDP;
        ip_send->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + reply_len);
        ip_send->saddr = ip->daddr; 
        ip_send->daddr = ip->saddr; // Меняем местами src и dst
        ip_send->check = 0;
        ip_send->id = 0;                
        ip_send->frag_off = 0;
        // Заполнение полей UDP заголовка ответа
        udp_send->source = htons(SERVER_PORT); udp_send->dest = udp->source;
        udp_send->len = htons(sizeof(struct udphdr) + reply_len);
        udp_send->check = 0; 

        // Заполнение структуры адреса назначения (cli_addr)
        struct sockaddr_in cli_addr;
        memset(&cli_addr, 0, sizeof(cli_addr));
        cli_addr.sin_family = AF_INET;
        cli_addr.sin_port = udp_send->dest; // Назначаем порт получателя 
        cli_addr.sin_addr.s_addr = ip_send->daddr; // Назначаем IP адрес получателя
        
        // Отправка пакета клиенту
        sendto(fd, send_buf, ntohs(ip_send->tot_len), 0, (struct sockaddr*)&cli_addr, sizeof(cli_addr));
    }

    printf("\nЗавершение работы сервера\n");
    free_client_list(&c_list);
    close(fd); 
    return 0;
}
