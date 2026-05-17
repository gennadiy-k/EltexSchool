#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include "gui.h"       
#include "message.h"   

void* client_receiver_thread(void* arg) {

    Chat_wins* gui = (Chat_wins*)arg;
    Message rcv_msg;
    long my_pid = (long)getpid();

    int msqid = gui->msqid; 
    
    // Цикл ожидания данных от сервера
    while (1) {
        if (msgrcv(msqid, &rcv_msg, sizeof(Message) - sizeof(long), my_pid, 0) == -1) {
            break; 
        }
       
        pthread_mutex_lock(&gui->mutex);

        if (rcv_msg.type == MSG_TEXT) {
            wprintw(gui->chat_subwin, "[%s]: %s\n", rcv_msg.username, rcv_msg.text);
        } 

        else if (rcv_msg.type == MSG_CONNECT || rcv_msg.type == MSG_OLD_CONNECT) {

            // Печатаем надпись в чат только если это новое подключение
            if (rcv_msg.type == MSG_CONNECT) {
                wprintw(gui->chat_subwin, ">>> %s joined the chat <<<\n", rcv_msg.username);
            }
                
            if (gui->total_users < MAX_USERS) {
                mvwprintw(gui->users_win, gui->total_users + 1, 1, "%s", rcv_msg.username);
                gui->total_users++;
            }
        }        

        touchwin(gui->chat_win);
        wrefresh(gui->chat_win);
        wrefresh(gui->chat_subwin);
        wrefresh(gui->users_win);

        wmove(gui->input_win, 1, 2); 
        wrefresh(gui->input_win);
        
        pthread_mutex_unlock(&gui->mutex);
    }    
    return NULL;

}
   




