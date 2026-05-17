#ifndef MESSAGE_H
#define MESSAGE_H

#define N 10

typedef struct msgbuf {
    long mtype;
    char mtext[N];
} Message;

#endif