#ifndef MESSAGE_H
#define MESSAGE_H

#include <semaphore.h>

#define MAX_HISTORY 20      // Количество сообщений (сохраненных)
#define MAX_USERS 10        // Количество пользователей
#define SHM_SERVER "/shm_server"
#define SEM_SERVER "/sem_server"
#define SEM_SERVER_CNT "/sem_server_cnt"
#define SEM_CLIENT "/sem_client"

typedef struct {
    int flag;
    sem_t *sem_srv_cnt;
} ServerCtrl;

typedef enum {
    MSG_CONNECT,        // Пользователь подключается (передает имя)
    MSG_TEXT            // Текстовое сообщение     
} EventType;

typedef struct {           
    EventType type;         // Тип события
    pid_t client_pid;       // Идентификатор пользователя
    char username[32];      // Имя пользователя
    char text[256];         //Текст сообщения
} Message;

typedef struct {
    Message input_slot;

    char active_users[MAX_USERS][32];   // Список ползователей
    int total_users;

    Message chat_history[MAX_HISTORY];   
    int history_cnt;                    // Счетчик сообщений в чате  
} ChatSHM;

void* client_receiver_thread(void* arg);
void* server_console_thread(void* arg);

#endif