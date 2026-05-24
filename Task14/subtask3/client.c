#define _DEFAULT_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <pthread.h>
#include "gui.h"
#include "message.h"

int main() {

    Message msg;
    Chat_wins gui;
    pthread_t TID;

    char input_buf[256]; // Буфер для ввода текста
    int flag = 1;
    char login_user[32]; // Буфер для хранения имени пользователя
    
    int shm_fd = shm_open(SHM_SERVER, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Ошибка вызова shm_open (client)");
        exit(EXIT_FAILURE);        
    }
    ChatSHM* shmp = mmap(NULL, sizeof(ChatSHM), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t* sem_srv = sem_open(SEM_SERVER, 0);
    sem_t* sem_srv_cnt = sem_open(SEM_SERVER_CNT, 0);
    if (sem_srv == SEM_FAILED || sem_srv_cnt == SEM_FAILED) {
        perror("sem_open failed (client)");
        exit(EXIT_FAILURE);
    }

    gui.shmp = shmp;
    init_curses(&gui);
    
    // Блок ввода имени пользователя
    while (flag) {
        pthread_mutex_lock(&gui.mutex);
        wrefresh(gui.chat_subwin); 
        mvwprintw(gui.input_win, 1, 2, "Input your username to join the chat: ");
        wrefresh(gui.input_win);
        echo();
        pthread_mutex_unlock(&gui.mutex);
        wgetnstr(gui.input_win, login_user, 31);
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

    msg.type = MSG_CONNECT;
    msg.client_pid = getpid();
    strcpy(msg.username, login_user); 
    memset(msg.text, 0, sizeof(msg.text));

    sem_wait(sem_srv);
    shmp->input_slot = msg;
    sem_post(sem_srv);
    sem_post(sem_srv_cnt);

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

        msg.type = MSG_TEXT;
        msg.client_pid = getpid();
        strcpy(msg.username, login_user);
        strcpy(msg.text, input_buf);

        sem_wait(sem_srv);
        shmp->input_slot = msg;
        sem_post(sem_srv);
        sem_post(sem_srv_cnt);

        pthread_mutex_lock(&gui.mutex);
        draw_input_window(&gui);
        pthread_mutex_unlock(&gui.mutex);
    }

    pthread_cancel(TID);
    pthread_join(TID, NULL);

    sem_close(sem_srv);
    sem_close(sem_srv_cnt);
    munmap(shmp, sizeof(ChatSHM));

    delwin(gui.chat_subwin);
    delwin(gui.chat_win);
    delwin(gui.users_win);
    delwin(gui.input_win);
    endwin();

    return 0;
}
