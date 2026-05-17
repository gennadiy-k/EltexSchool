#ifndef MESSAGE_H
#define MESSAGE_H

#define MAX_HISTORY 50 // Количество сообщений (сохраненных)


typedef enum {
    MSG_CONNECT,        // Пользователь подключается (передает имя)
    MSG_OLD_CONNECT,    // Старый пользователь
    MSG_TEXT,           // Текстовое сообщение
    MSG_SYSTEM,         // Системное уведомление 
    MSG_DISCONNECT      // Клиент выходит
} EventType;

typedef struct msgbuf {
    long mtype;             
    EventType type;         // Тип события
    long client_pid;        // Идентификатор пользователя
    char username[64];      // Имя пользователя
    char text[256];         //Текст сообщения
} Message;

void* client_receiver_thread(void* arg);

#endif