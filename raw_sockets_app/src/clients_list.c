#include <stdlib.h>
#include "clients_list.h"

// Функция для поиска и создания узла в списке
int reg_client_msg(ClientList* list, uint32_t ip, uint16_t port) {
    // Указатель на начало списка
    ClientNode* curr = list->head; 
    
    while (curr != NULL) {   
        // Проверям совпадает ли IP и порт текущего клиента с отправителем пакета
        if (curr->ip == ip && curr->port == port) {
            curr->msg_cnt++; // Если совпали, то увеличиваем счетчик сообщений на 1
            return curr->msg_cnt;
        }
        // Переходим к следующему
        curr = curr->next; 
    }
    
    // Если цикл никого не нашел, то выделяем память под новый узел
    ClientNode* new_node = (ClientNode*)malloc(sizeof(ClientNode));
    
    // Заполняем поля данными клиента
    new_node->ip = ip;
    new_node->port = port;
    new_node->msg_cnt = 1;
    
    // Вставляем нового клиента в начало списка
    new_node->next = list->head; 
    new_node->prev = NULL;
    
    // Корректируем указатель prev прошлого начального узла
    if (list->head != NULL) {
        list->head->prev = new_node;
    }
    
    // Делаем новый узел началом списка
    list->head = new_node;
    
    return 1;
}

// Функция удаления клиента из списка при отключении
void remove_client(ClientList* list, uint32_t ip, uint16_t port) {
    // Проходим по списку в поисках нужного клиента
    ClientNode* curr = list->head;
    while (curr != NULL) {
        // Ищем нужного клиента по его IP и порту
        if (curr->ip == ip && curr->port == port) {
            
            // Если перед удаляемым узлом кто-то есть
            if (curr->prev != NULL) {
                // Предыдущий узел указывает на следующий
                curr->prev->next = curr->next;
            }
            else {
                // Если удаляем первый, то ставим в начало списка следующий узел
                list->head = curr->next;
            }
            
            // Если за удаляемым узлом тоже кто-то есть
            if (curr->next != NULL) {
                // Тогда следующий узел указывает на предыдущий
                curr->next->prev = curr->prev;
            }
            
            free(curr); 
            return;
        }
        
        // Переходим к следующему узлу
        curr = curr->next;
    }
}

// Функция очищает всю память при выключении сервера
void free_client_list(ClientList* list) {
    ClientNode* curr = list->head;
    
    while (curr != NULL) { 
        ClientNode* next_node = curr->next; // Запоминаем адрес следующего узла
        free(curr);
        curr = next_node;
    }

    list->head = NULL;
}
