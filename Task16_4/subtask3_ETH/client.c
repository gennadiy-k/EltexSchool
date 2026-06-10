#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <net/ethernet.h>  
#include <arpa/inet.h>
#include <net/if.h>        
#include <linux/if_packet.h> 
#include "socket_in.h"

// Функция расчета чексуммы
unsigned short calc_checksum(unsigned short *ptr) {
    int csum = 0;

    for (int i = 0; i < 10; i++) { 
        csum = csum + ntohs(*ptr); 
        ptr++;
    }

    int tmp = csum >> 16;         
    csum = (csum & 0xFFFF) + tmp; 
    return htons((unsigned short)(~csum)); 
}


int main() {
    char buf[BUF_SIZE];
    
    struct ethhdr* eth = (struct ethhdr*) buf;
    struct iphdr* ip = (struct iphdr*) (buf + sizeof(struct ethhdr));
    struct udphdr* udp = (struct udphdr*) (buf + sizeof(struct ethhdr) + sizeof(struct iphdr));
    char* payload = buf + sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr);

    memcpy(payload, "ABCD", 5);
    int payload_len = strlen(payload);
    int udp_packet_len = sizeof(struct udphdr) + payload_len;
    int ip_packet_len = sizeof(struct iphdr) + udp_packet_len;
    int full_packet_len = sizeof(struct ethhdr) + ip_packet_len;

    // Заполняем Ethernet (канальный уровень)
    unsigned char dest_mac[6] = {0xba, 0xfa, 0x46, 0xd2, 0xf3, 0x65}; // MAC адрес контейнера-сервера
    memcpy(eth->h_dest, dest_mac, 6);
    memset(eth->h_source, 0x00, 6); 
    eth->h_proto = htons(ETH_P_IP);

    // Заполняем IP (сетевой уровень)
    ip->version = 4; 
    ip->ihl = 5;
    ip->tos = 0;
    ip->tot_len = htons(ip_packet_len); 
    ip->id = 0;
    ip->frag_off = 0;
    ip->ttl = 10;
    ip->protocol = IPPROTO_UDP;

    if (inet_pton(AF_INET, CLIENT_IP, &ip->saddr) <= 0 || inet_pton(AF_INET, SERVER_IP, &ip->daddr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    ip->check = 0;
    ip->check = calc_checksum((unsigned short *)ip);

    // Заполняем UDP (транспортный уровень)
    udp->uh_sport = htons(CLIENT_PORT);
    udp->uh_dport = htons(SERVER_PORT);
    udp->uh_ulen = htons(udp_packet_len);
    udp->uh_sum = 0; 

    int fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = if_nametoindex(INTERFACE_NAME); // Привязываемся к сетевому интерфейсу (eth1) контейнера

    if (sendto(fd, buf, full_packet_len, 0, (struct sockaddr*)&sll, sizeof(sll)) == -1) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
    printf("[КЛИЕНТ] Серверу отправлен пакет со строкой: %s\n", payload);

    // Ожидание ответа
    char recv_buf[BUF_SIZE];
    int headers_size = sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr);

    while (1) {
        memset(recv_buf, 0, sizeof(recv_buf));
        ssize_t nread = recvfrom(fd, recv_buf, sizeof(recv_buf), 0, NULL, NULL);
        if (nread == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        if (nread >= headers_size) {
            struct ethhdr* r_eth = (struct ethhdr*) recv_buf;
            if (ntohs(r_eth->h_proto) == ETH_P_IP) {
                struct iphdr* r_ip = (struct iphdr*) (recv_buf + sizeof(struct ethhdr));
                if (r_ip->protocol == IPPROTO_UDP) {
                    struct udphdr* r_udp = (struct udphdr*) (recv_buf + sizeof(struct ethhdr) + sizeof(struct iphdr));
                    // Проверяем соответствие портов клиента и сервера
                    if (ntohs(r_udp->uh_dport) == CLIENT_PORT && ntohs(r_udp->uh_sport) == SERVER_PORT) {
                        char* data = recv_buf + headers_size;
                        int data_len = nread - headers_size;
                        data[data_len] = '\0';
                        printf("[КЛИЕНТ] Получено сообщение: %s\n", data);
                        break;
                    }
                }
            }
        }
    }

    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }
    return 0;
}
