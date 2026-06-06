#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h> 
#include "socket_in.h"

// TCP подключение
void tcp_client() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char recv_buf[BUFFER_SIZE];

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) { 
        perror("TCP socket"); 
        return; 
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP_ADDR, &server_addr.sin_addr);

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("TCP connect");
        close(sock_fd);
        return;
    }

    send(sock_fd, "time", 4, 0);

    int nread = recv(sock_fd, recv_buf, sizeof(recv_buf) - 1, 0);
    if (nread > 0) {
        recv_buf[nread] = '\0';
        recv_buf[strcspn(recv_buf, "\n")] = '\0'; 
        printf("TCP | Время: %s\n", recv_buf);
    }
    else {
        printf("Ошибка получения данных\n");
    }

    close(sock_fd);
}

// UDP подключение
void udp_client() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char recv_buf[BUFFER_SIZE];
    socklen_t addr_len = sizeof(server_addr);

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) { 
        perror("UDP socket"); 
        return; 
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, IP_ADDR, &server_addr.sin_addr);

    sendto(sock_fd, "time", 4, 0, (struct sockaddr *)&server_addr, addr_len);

    int nread = recvfrom(sock_fd, recv_buf, sizeof(recv_buf) - 1, 0, (struct sockaddr *)&server_addr, &addr_len);
    if (nread > 0) {
        recv_buf[nread] = '\0';
        recv_buf[strcspn(recv_buf, "\n")] = '\0'; 
        printf("UDP | Время: %s\n", recv_buf);
    }
    else {
        printf("Ошибка получения данных\n");
    }

    close(sock_fd);
}

void* tcp_thread(void* arg) {
    tcp_client();
    return NULL;
}

void* udp_thread(void* arg) {
    udp_client();
    return NULL;
}

// Функция запуска пула потоков
void test_threads(int is_tcp) {
    pthread_t threads[CLIENTS_NUM];
    
    printf("Запуск %d потоков\n", CLIENTS_NUM);

    // Создаем потоки
    for (int i = 0; i < CLIENTS_NUM; i++) {
        if (is_tcp) {
            pthread_create(&threads[i], NULL, tcp_thread, NULL);
        } 
        else {
            pthread_create(&threads[i], NULL, udp_thread, NULL);
        }
    }

    for (int i = 0; i < CLIENTS_NUM; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Все потоки завершили работу\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Для запуска: ./client <tcp/udp/test_tcp/test_udp>\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "tcp") == 0) {
        tcp_client();
    } 
    else if (strcmp(argv[1], "udp") == 0) {
        udp_client();
    } 
    else if (strcmp(argv[1], "test_tcp") == 0) {
        test_threads(1); // Тест подключений TCP
    } 
    else if (strcmp(argv[1], "test_udp") == 0) {
        test_threads(0); // Тест подключений UDP
    } 
    else {
        printf("Неверный аргумент. Ввести: ./client <tcp/udp/test_tcp/test_udp>\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
