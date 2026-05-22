#ifndef GUI_H
#define GUI_H

#include <pthread.h>
#include <ncurses.h>
#include "message.h"


/* Определение структуры для 3-х окон */
typedef struct windows {
    WINDOW *win;
    WINDOW *chat_win;
    WINDOW *chat_subwin; // Внутренне окно (для прокрутки), родительское - chat_win
    WINDOW *users_win;
    WINDOW *input_win;
    pthread_mutex_t mutex;

    Message active_users[MAX_USERS];
    int total_users;

    int msqid;
} Chat_wins;


/* Инициализация ncurses */
void init_curses(Chat_wins* gui);

/* Прототип функции отрисовки (обновления) окна ввода */
void draw_input_window(Chat_wins* gui);

#endif