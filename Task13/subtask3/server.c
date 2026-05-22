#define _DEFAULT_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <sys/ipc.h>    
#include <sys/msg.h> 
#include <errno.h>

#include "gui.h"
#include "message.h"


int main() {
    
    pthread_t console_TID;
    Message active_users[MAX_USERS];
    Message msg;

    int total_users = 0;

    char chat_history[MAX_HISTORY][256];
    char history_aut[MAX_HISTORY][64];
    int history_head = 0;
    int history_size = 0;

    ServerCtrl ctrl;
    ctrl.flag = 1; 
    ctrl.msqid = 0;

    key_t key = ftok("./server", 1);
    if (key == -1) {
        perror("Ошибка вызова ftok (server)");
        exit(EXIT_FAILURE);
    }

    ctrl.msqid = msgget(key, IPC_CREAT | 0666);
    if (ctrl.msqid == -1) {
        perror("Ошибка вызова msgget (server)");
        exit(EXIT_FAILURE);
    }

    printf("[СЕРВЕР] Очередь создана. ID: %d\n", ctrl.msqid);
    printf("[СЕРВЕР] Введи 'exit' в консоль для закрытия.\n");

    pthread_create(&console_TID, NULL, server_console_thread, (void*)&ctrl); // Создаем поток для обработки ввода комманды exit в консоли

    while (ctrl.flag) {
        // Читаем из очереди сообщения (сообщения к серверу)
        if (msgrcv(ctrl.msqid, &msg, sizeof(Message) - sizeof(long), 1, 0) == -1) {
            if (!ctrl.flag) {
                break;
            }
            perror("Ошибка вызова msgrcv (server)");
            break;
        }
        // Обрабатываем подключение нового пользователю
        if (msg.type == MSG_CONNECT) {
            if (total_users < MAX_USERS) {
                long new_client_pid = msg.client_pid; 

                // Отправляем новому пользователю список тех, кто в сети
                Message old_user_msg;
                old_user_msg.type = MSG_OLD_CONNECT; 
                
                for (int i = 0; i < total_users; i++) {
                    old_user_msg.mtype = new_client_pid; 
                    old_user_msg.client_pid = active_users[i].client_pid;
                    strcpy(old_user_msg.username, active_users[i].username);
                    memset(old_user_msg.text, 0, sizeof(old_user_msg.text)); 
                    msgsnd(ctrl.msqid, &old_user_msg, sizeof(Message) - sizeof(long), 0);
                }

                // Сохраняем нового пользователя в массив
                active_users[total_users] = msg;
                printf("[СЕРВЕР] Подключен: %s. Всего онлайн: %d\n", msg.username, total_users + 1);
                total_users++;                
                
                // Отправляем новому пользователю историю текстовых сообщений из  буфера
                Message history_msg;
                history_msg.type = MSG_TEXT; 
                history_msg.client_pid = 0; 

                int start_index = 0;
                if (history_size == MAX_HISTORY) {
                    start_index = history_head;
                }

                for (int j = 0; j < history_size; j++) {
                    int current_index = (start_index + j) % MAX_HISTORY;

                    history_msg.mtype = new_client_pid; 
                    strcpy(history_msg.username, history_aut[current_index]);
                    strcpy(history_msg.text, chat_history[current_index]);
                    
                    msgsnd(ctrl.msqid, &history_msg, sizeof(Message) - sizeof(long), 0);
                }

                // Оповещаем всех участников о входе нового пользователя
                for (int i = 0; i < total_users; i++) {
                    msg.mtype = active_users[i].client_pid; 
                    msg.type = MSG_CONNECT; 
                    msgsnd(ctrl.msqid, &msg, sizeof(Message) - sizeof(long), 0);
                }
            }
        }

        // Обработка сообщений в чате
        else if (msg.type == MSG_TEXT) {
            printf("[СЕРВЕР] Чат -> [%s]: %s\n", msg.username, msg.text);

            // Сохраняем пришедшее сообщение в буфер истории
            strcpy(chat_history[history_head], msg.text);
            strcpy(history_aut[history_head], msg.username);

            // Сдвигаем индекс
            history_head = (history_head + 1) % MAX_HISTORY;

            
            if (history_size < MAX_HISTORY) {
                history_size++;
            }               
            // Рассылаем копию сообщения всем активным пользователям по их pid
            for (int i = 0; i < total_users; i++) {
                long recv_pid = active_users[i].client_pid;
                msg.mtype = recv_pid;
                if (msgsnd(ctrl.msqid, &msg, sizeof(Message) - sizeof(long), 0)) {
                    perror("Ошибка отправки msgsnd (server)");
                }
            }
        }
    }

    pthread_join(console_TID, NULL);
    printf("[СЕРВЕР] Завершение работы.\n");
    return 0;
}