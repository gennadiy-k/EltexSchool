#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include "socket_in.h"


void sig_handler(int sig) {
    // Пустой обработчик чтобы прервать блокирующие функции (fgets/recvfrom) и получить EINTR
}

// Функция отправки пакета серверу
void send_data_udp(int fd, struct sockaddr_in serv_addr, uint32_t src_ip, uint16_t src_port, const char* payload) {
    char send_buf[BUFFER_SIZE]; 
    memset(send_buf, 0, BUFFER_SIZE);
    int payload_len = strlen(payload);

    // Устанавливаем указатели на начало IP и UDP заголовков внутри буфера
    struct iphdr *ip = (struct iphdr *) send_buf;
    struct udphdr *udp = (struct udphdr *) (send_buf + sizeof(struct iphdr));

    // Копируем сообщение после заголовков
    memcpy(send_buf + sizeof(struct iphdr) + sizeof(struct udphdr), payload, payload_len);

    // Заполнение заголовка IP
    ip->ihl = 5;
    ip->version = 4;
    ip->ttl = 10;
    ip->protocol = IPPROTO_UDP;
    ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + payload_len);
    // Копируем адреса из структуры serv_addr
    ip->saddr = src_ip;
    ip->daddr = serv_addr.sin_addr.s_addr;
    ip->check = 0;
    ip->id = 0;
    ip->frag_off = 0;

    // Заполнение заголовка UDP 
    udp->source = htons(src_port); // Порт клиента 
    udp->dest = serv_addr.sin_port; // Порт назначения сервера 
    udp->len = htons(sizeof(struct udphdr) + payload_len); // Длина UDP и данных
    udp->check = 0;

    // Отправляем пакет серверу
    sendto(fd, send_buf, ntohs(ip->tot_len), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
}

int main(void) {
    struct sockaddr_in serv_addr;
    uint16_t client_port;
    uint32_t client_ip;
    
    // Создаем порт на основе PID
    pid_t pid = getpid();
    client_port = (uint16_t)(20000 + (getpid() % 40000));

    // Настройка структуры адреса назначения сервера
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    
    if (inet_pton(AF_INET, IP_ADDR, &serv_addr.sin_addr) <= 0 || inet_pton(AF_INET, IP_ADDR, &client_ip) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (fd < 0) { 
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1; 
    setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &opt, sizeof(opt));

    // Обработчик для SIGINT
    struct sigaction handler;
    memset(&handler, 0, sizeof(handler));
    handler.sa_handler = sig_handler; 
    sigemptyset(&handler.sa_mask);
    sigaction(SIGINT, &handler, NULL);

    printf("Клиент подключен. Порт: %d\nДля выхода использовать Ctrl+C\n", client_port);
    char msg[BUFFER_SIZE], recv_buf[BUFFER_SIZE];

    while (1) {
        printf(">: ");

        if (!fgets(msg, sizeof(msg), stdin)) {
            // Если нажали Ctrl+C во время ожидания ввода строки
            if (errno == EINTR) {
                break;
            }
            break;
        }

        msg[strcspn(msg, "\n")] = 0;
        if (strlen(msg) == 0) {
            continue;
        }
        // Отправляем пакет серверу
        send_data_udp(fd, serv_addr, client_ip, client_port, msg);

        int flag = 0;
        while (!flag) {
            ssize_t res = recvfrom(fd, recv_buf, BUFFER_SIZE, 0, NULL, NULL);
            if (res < 0) {
                if (errno == EINTR) {
                    break; 
                }
                break;
            }

            struct iphdr* ip = (struct iphdr*)recv_buf;
            struct udphdr* udp = (struct udphdr*)(recv_buf + sizeof(struct iphdr));

            // Фильтрация трафика: пакет должен быть от нашего сервера и назначен на наш порт (client_port)
            if (ip->saddr == serv_addr.sin_addr.s_addr && ntohs(udp->source) == SERVER_PORT && ntohs(udp->dest) == client_port) {
                
                // Находим указатель на начало данных (за UDP заголовком)
                char *payload = recv_buf + sizeof(struct iphdr) + sizeof(struct udphdr);
                int payload_len = ntohs(udp->len) - sizeof(struct udphdr);
                
                // Защита от переполнения
                if (payload_len >= 0 && payload_len < BUFFER_SIZE) {
                    payload[payload_len] = '\0';
                    printf("Ответ: %s\n", payload);
                }
                flag = 1;
            }
        }

        if (errno == EINTR) {
            break;
        }
    }

    send_data_udp(fd, serv_addr, client_ip, client_port, DISCONNECT);

    close(fd); 
    printf("\nЗавершение работы клиента\n");
    return 0;
}
