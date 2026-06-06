#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include "socket_in.h"

volatile sig_atomic_t flag = 1;

void sig_handler(int sig_num) {
    flag = 0; 
}

int main() {
    // Обработчик для SIGINT
    struct sigaction handler;
    memset(&handler, 0, sizeof(handler)); 
    sigemptyset(&handler.sa_mask);
    handler.sa_handler = sig_handler; 
    sigaction(SIGINT, &handler, NULL);

    int tcp_fd, udp_fd;
    struct sockaddr_in server_addr;

    // Настройка TCP сокета
    tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_fd < 0) { 
        perror("TCP socket"); 
        exit(EXIT_FAILURE); 
    }

    int opt = 1;
    setsockopt(tcp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    server_addr.sin_port = htons(PORT);

    if (bind(tcp_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("TCP bind"); 
        exit(EXIT_FAILURE);
    }
    if (listen(tcp_fd, 5) < 0) {
        perror("listen"); 
        exit(EXIT_FAILURE);
    }

    // Настройка UDP сокета
    udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_fd < 0) { perror("UDP socket"); 
        exit(EXIT_FAILURE);
    }

    if (bind(udp_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("UDP bind"); 
        exit(EXIT_FAILURE);
    }

    struct pollfd fds[2 + MAX_CLIENTS];
    int nfds = 2; 

    fds[0].fd = tcp_fd;
    fds[0].events = POLLIN;

    fds[1].fd = udp_fd;
    fds[1].events = POLLIN;

    for (int i = 2; i < 2 + MAX_CLIENTS; i++) {
        fds[i].fd = -1; 
    }

    printf("Сервер запущен\n");

    while (flag) {
        int poll_count = poll(fds, nfds, -1); 
        if (poll_count < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("poll"); 
            exit(EXIT_FAILURE); 
        }

        int cur_nfds = nfds; // Сохраняем число дескрипторов
        for (int i = 0; i < cur_nfds; i++) {
            if (!(fds[i].revents & POLLIN)) continue;

            // Новое подключение по TCP
            if (fds[i].fd == tcp_fd) {
                struct sockaddr_in client_addr;
                socklen_t addr_len = sizeof(client_addr);
                int client_fd = accept(tcp_fd, (struct sockaddr *)&client_addr, &addr_len);
                
                if (client_fd >= 0) {
                    int client_reg = 0;
                    for (int j = 2; j < 2 + MAX_CLIENTS; j++) {
                        if (fds[j].fd == -1) {
                            fds[j].fd = client_fd;
                            fds[j].events = POLLIN;
                            if (j >= nfds) {
                                nfds = j + 1;
                            }
                            client_reg = 1;
                            break;
                        }
                    }

                    if (!client_reg) {
                        printf("Достигнут лимит клиентов\n");
                        close(client_fd);
                    }
                }
            }
            
            // Новое подключение по UDP
            else if (fds[i].fd == udp_fd) {
                struct sockaddr_in client_addr;
                socklen_t addr_len = sizeof(client_addr);
                char recv_buf[BUFFER_SIZE];
                char time_buf[32];

                int nread = recvfrom(udp_fd, recv_buf, sizeof(recv_buf) - 1, 0, (struct sockaddr *)&client_addr, &addr_len);
                if (nread >= 0) {
                    // Отправка времени
                    time_t cur_time = time(NULL);
                    ctime_r(&cur_time, time_buf);

                    sendto(udp_fd, time_buf, strlen(time_buf), 0, (struct sockaddr *)&client_addr, addr_len);
                }
            }
            
            // Запрос от подключенного TCP-клиента
            else {
                char recv_buf[BUFFER_SIZE];
                char time_buf[32];
                int client_fd = fds[i].fd;

                int nread = recv(client_fd, recv_buf, sizeof(recv_buf) - 1, 0);
                if (nread > 0) {
                    // Отправка времени
                    time_t cur_time = time(NULL);
                    ctime_r(&cur_time, time_buf);

                    send(client_fd, time_buf, strlen(time_buf), 0);
                }
                
                close(client_fd);
                fds[i].fd = -1;
            }
        }

        // Очистка массива от закрытых клиентов
        while (nfds > 2 && fds[nfds - 1].fd == -1) {
            nfds--;
        }
    }
    printf("\nПолучен сигнал завершения работы сервера\n");

    for (int i = 2; i < nfds; i++) {
        if (fds[i].fd != -1) {
            close(fds[i].fd);
        }
    }
    close(tcp_fd);
    close(udp_fd);

    printf("Завершение работы\n");
    return 0;
}
