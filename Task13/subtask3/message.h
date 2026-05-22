#ifndef MESSAGE_H
#define MESSAGE_H

#define MAX_HISTORY 50 // Количество сообщений (сохраненных)
#define MAX_USERS 10   // Количество пользователей 

typedef struct {
    int flag;
    int msqid;
} ServerCtrl;

typedef enum {
    MSG_CONNECT,        // Пользователь подключается (передает имя)
    MSG_OLD_CONNECT,    // Старый пользователь
    MSG_TEXT            // Текстовое сообщение     
} EventType;

typedef struct msgbuf {
    long mtype;             
    EventType type;         // Тип события
    long client_pid;        // Идентификатор пользователя
    char username[64];      // Имя пользователя
    char text[256];         //Текст сообщения
} Message;

void* client_receiver_thread(void* arg);
void* server_console_thread(void* arg);
#endif