#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include "gui.h"       
#include "message.h"  

void* client_receiver_thread(void* arg) {
    Chat_wins* gui = (Chat_wins*)arg;
    ChatSHM* shmp = gui->shmp;

    sem_t* sem_srv = sem_open(SEM_SERVER, 0);
    sem_t* sem_cli = sem_open(SEM_CLIENT, 0);
    if (sem_srv == SEM_FAILED || sem_cli == SEM_FAILED) {
        perror("Ошибка sem_open (thread)");
        return NULL;  
    }
    
    int cur_hist_cnt = 0; // Локальный счетчик выведенных сообщений на экран пользователя
    int last_users_cnt = 0; // Локальный счетчик выведенныхэ пользователей на экран (чтобы не перерисовывать окно)

    int first_run_flag = 1; 

    // Цикл ожидания данных от сервера
    while (1) {
        if (first_run_flag) {
            first_run_flag = 0;
        }
        else {
            sem_wait(sem_cli);
        }

        sem_wait(sem_srv);
        int refresh_chat = (cur_hist_cnt < shmp->history_cnt);
        int refresh_users = (last_users_cnt != shmp->total_users);
        sem_post(sem_srv);

        if (refresh_chat || refresh_users) {

            pthread_mutex_lock(&gui->mutex);

            if (refresh_chat) {
                sem_wait(sem_srv);

                if (shmp->history_cnt > MAX_HISTORY && cur_hist_cnt < shmp->history_cnt - MAX_HISTORY) {
                    cur_hist_cnt = shmp->history_cnt - MAX_HISTORY;
                }

                // Вывод новых сообщений
                while (cur_hist_cnt < shmp->history_cnt) {
                    int i = cur_hist_cnt % MAX_HISTORY; // Индекс массива истории чата (для ограничения вывода на сообщений на экран)

                    Message rcv_msg = shmp->chat_history[i];
                    if (rcv_msg.username[0] != '\0') {
                        if (rcv_msg.type == MSG_TEXT) {
                            wprintw(gui->chat_subwin, "[%s]: %s\n", rcv_msg.username, rcv_msg.text);
                        } 
                        else if (rcv_msg.type == MSG_CONNECT) {
                            wprintw(gui->chat_subwin, ">>> %s joined the chat <<<\n", rcv_msg.username);
                        }
                    }
                    cur_hist_cnt++;
                }
                sem_post(sem_srv);
            }
            if (refresh_users) {

                wclear(gui->users_win);
                box(gui->users_win, 0, 0); 
                mvwprintw(gui->users_win, 0, 2, " ONLINE MEMBERS ");

                sem_wait(sem_srv);
                // Вывод пользователей из памяти на экран
                for (int i = 0; i < shmp->total_users && shmp->active_users[i][0] != '\0'; i++) {
                        mvwprintw(gui->users_win, i + 1, 1, "%s", shmp->active_users[i]);
                }
                last_users_cnt = shmp->total_users;
                sem_post(sem_srv);
            }

            touchwin(gui->chat_win);
            wrefresh(gui->chat_win);
            wrefresh(gui->chat_subwin);
            wrefresh(gui->users_win);

            wmove(gui->input_win, 1, 2); 
            wrefresh(gui->input_win);
            
            pthread_mutex_unlock(&gui->mutex);
        }

        sem_post(sem_cli);
    }
    return NULL;
}

void* server_console_thread(void* arg) {

    ServerCtrl* ctrl = (ServerCtrl*)arg;
    char command[20];
    
    while (ctrl->flag) { 
        memset(command, 0, sizeof(command));
        
        if (fgets(command, sizeof(command), stdin) == NULL) {
            continue;
        }

        command[strcspn(command, "\n")] = 0; 

        if (strcmp(command, "exit") == 0) {
            ctrl->flag = 0; 

            sem_post(ctrl->sem_srv_cnt);
            break;
        }
    }
    return NULL;
}