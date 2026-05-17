#define _DEFAULT_SOURCE 
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "gui.h"
#include "message.h"

void init_curses(Chat_wins* gui) {
    
    initscr();              
    noecho();               
    curs_set(1);    
    cbreak();               
    keypad(stdscr, TRUE);   

    // Инициализируем мьютекс
    pthread_mutex_init(&gui->mutex, NULL);

    gui->total_users = 0; 

    // Создаем окна
    gui->win  = stdscr;
    gui->chat_win  = newwin(LINES - 3, COLS - 20, 0, 0);
    gui->users_win = newwin(LINES - 3, 20, 0, COLS - 20);
    gui->input_win = newwin(3, COLS, LINES - 3, 0);

    gui->chat_subwin = derwin(gui->chat_win, LINES - 5, COLS - 22, 1, 1);
    
    scrollok(gui->chat_subwin, TRUE);    

    // Рамки
    box(gui->chat_win, 0, 0);   
    box(gui->users_win, 0, 0);  
    box(gui->input_win, 0, 0);  

    // Заголовки окон
    mvwprintw(gui->chat_win, 0, 2, " CHATROOM ");
    mvwprintw(gui->users_win, 0, 2, " ONLINE MEMBERS ");
    mvwprintw(gui->input_win, 0, 2, " Type a message (or /quit to exit) ");

    wrefresh(gui->chat_win);
    wrefresh(gui->users_win);
    wrefresh(gui->input_win);

}


void draw_input_window(Chat_wins* gui) {

    wmove(gui->input_win, 1, 2);
    wclrtoeol(gui->input_win);
    box(gui->input_win, 0, 0);
    mvwprintw(gui->input_win, 0, 2, " Type a message (or /quit to exit) ");
    wrefresh(gui->input_win); 

}