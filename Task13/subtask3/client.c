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
#include "gui.h"
#include "message.h"

int main() {

    Message msg;
    Chat_wins gui;

    pthread_t TID;

    char input_buf[256]; // Буфер для ввода текста
    int flag = 1;
    char login_user[32]; // Буфер для хранения имени пользователя
    
    key_t key = ftok("./server", 1);
    if (key == -1) {
        perror("Ошибка вызова ftok (client)");
        exit(EXIT_FAILURE);
    }

    int msqid = msgget(key, 0666);
    if (msqid == -1) {
        perror("Ошибка вызова msgget (client)");
        exit(EXIT_FAILURE);
    }

    gui.msqid = msqid; 

    init_curses(&gui);
    
    // Блок ввода имени пользователя
    while (flag) {
        pthread_mutex_lock(&gui.mutex);
        wrefresh(gui.chat_subwin); 
        mvwprintw(gui.input_win, 1, 2, "Input your username to join the chat: ");
        wrefresh(gui.input_win);
        echo();
        pthread_mutex_unlock(&gui.mutex);
        wgetnstr(gui.input_win, login_user, 255);
        pthread_mutex_lock(&gui.mutex);
        noecho();
        draw_input_window(&gui);
        if (strlen(login_user) == 0) {
            pthread_mutex_unlock(&gui.mutex);
            continue;
        }
        else {
            flag = 0;
        }
        pthread_mutex_unlock(&gui.mutex);
    }

    flag = 1; 
    msg.mtype = 1;
    msg.type = MSG_CONNECT;
    msg.client_pid = getpid();
    strcpy(msg.username, login_user); 
    msgsnd(msqid, &msg, sizeof(Message) - sizeof(long), 0);

    pthread_create(&TID, NULL, client_receiver_thread, (void*)&gui);

    // Цикл отправки сообщений 
    while (flag) {
        pthread_mutex_lock(&gui.mutex);
        wmove(gui.input_win, 1, 2); 
        echo();        
        pthread_mutex_unlock(&gui.mutex);

        wgetnstr(gui.input_win, input_buf, 255); // Ввод текста пользователем

        pthread_mutex_lock(&gui.mutex);
        noecho();
        pthread_mutex_unlock(&gui.mutex);

        if (strlen(input_buf) == 0) {
            pthread_mutex_lock(&gui.mutex);
            draw_input_window(&gui);
            pthread_mutex_unlock(&gui.mutex);
            continue;
        }

        if (strcmp(input_buf, "/quit") == 0) {
            flag = 0;
            continue;
        }

        msg.mtype = 1;
        msg.type = MSG_TEXT;
        msg.client_pid = getpid();
        strcpy(msg.username, login_user);
        strcpy(msg.text, input_buf);

        if (msgsnd(msqid, &msg, sizeof(Message) - sizeof(long), 0) == -1) {
            perror("Ошибка отправки сообщения (client)");
            break;            
        }

        pthread_mutex_lock(&gui.mutex);
        draw_input_window(&gui);
        pthread_mutex_unlock(&gui.mutex);
    }

    pthread_cancel(TID);
    pthread_join(TID, NULL);
    delwin(gui.chat_subwin);
    delwin(gui.chat_win);
    delwin(gui.users_win);
    delwin(gui.input_win);
    endwin();

    return 0;
}