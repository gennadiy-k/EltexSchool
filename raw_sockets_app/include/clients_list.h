#ifndef CLIENT_LIST_H
#define CLIENT_LIST_H

#include <stdint.h>
#include "socket_in.h"

// Структура узла двусвязного списка
typedef struct ClientNode {
    uint32_t ip; 
    uint16_t port; 
    int msg_cnt;
    struct ClientNode *next, *prev;
} ClientNode;

// Структура для управления двусвязным списком
typedef struct {
    ClientNode* head;
} ClientList;

// Прототипы функций для работы с двусвязным списком
int reg_client_msg(ClientList* list, uint32_t ip, uint16_t port);
void remove_client(ClientList* list, uint32_t ip, uint16_t port);
void free_client_list(ClientList* list);

#endif
