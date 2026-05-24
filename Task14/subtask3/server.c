#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include "message.h"

int main() {
    
    ServerCtrl ctrl;
    Message msg;
    pthread_t console_TID;

    
    shm_unlink(SHM_SERVER);
    sem_unlink(SEM_SERVER);
    sem_unlink(SEM_SERVER_CNT);
    sem_unlink(SEM_CLIENT);

    int shm_fd = shm_open(SHM_SERVER, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Ошибка shm_open (server)");
        exit(EXIT_FAILURE);
    }    

    sem_t* sem_srv = sem_open(SEM_SERVER, O_CREAT, 0666, 1); // Для защиты от одновременной записи
    sem_t* sem_srv_cnt = sem_open(SEM_SERVER_CNT, O_CREAT, 0666, 0); // Для блокировки сервера до появления новых сообщений
    sem_t *sem_cli = sem_open(SEM_CLIENT, O_CREAT, 0666, 0); // Для блокировки пользователей до появления новых сообщений
    if (sem_srv == SEM_FAILED || sem_srv_cnt == SEM_FAILED || sem_cli == SEM_FAILED) {
        perror("Ошибка sem_open (server)");
        exit(EXIT_FAILURE);       
    }

    if (ftruncate(shm_fd, sizeof(ChatSHM)) == -1) {
        perror("Ошибка ftruncate (server)");
        exit(EXIT_FAILURE);
    }

    ChatSHM* shmp = mmap(NULL, sizeof(ChatSHM), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shmp == MAP_FAILED) {
        perror("Ошибка mmap (server)");
        exit(EXIT_FAILURE);        
    }
    close(shm_fd);
    memset(shmp, 0, sizeof(ChatSHM));

    ctrl.flag = 1;               
    ctrl.sem_srv_cnt = sem_srv_cnt;

    printf("[СЕРВЕР] Чат запущен.\n");
    printf("[СЕРВЕР] Команда 'exit' для завершения работы.\n");

    pthread_create(&console_TID, NULL, server_console_thread, (void*)&ctrl); // Создаем поток для обработки ввода комманды exit в консоли

    while (ctrl.flag) {

        sem_wait(sem_srv_cnt);

        if(!ctrl.flag) {
            break;
        }

        sem_wait(sem_srv);
        msg = shmp->input_slot;
        sem_post(sem_srv);

        // Обрабатываем подключение нового пользователю
        if (msg.type == MSG_CONNECT) {
            sem_wait(sem_srv);
            
            if (shmp->total_users < MAX_USERS) {
                strcpy(shmp->active_users[shmp->total_users], msg.username); 
                shmp->total_users++;
                printf("[СЕРВЕР] Подключен: %s. Всего онлайн: %d\n", msg.username, shmp->total_users);

                // Сохраняем историю сообщений
                if (shmp->history_cnt < MAX_HISTORY) {
                    shmp->chat_history[shmp->history_cnt] = msg;
                }
                else {
                    int i = shmp->history_cnt % MAX_HISTORY; 
                    shmp->chat_history[i] = msg;
                }
                shmp->history_cnt++; 

                sem_post(sem_cli);
            }
            sem_post(sem_srv);
        }
        // Обработка сообщений в чате
        else if (msg.type == MSG_TEXT) {
            printf("[СЕРВЕР] Чат -> [%s]: %s\n", msg.username, msg.text);

            sem_wait(sem_srv);
            
            // Сохраняем историю сообщений
            if (shmp->history_cnt < MAX_HISTORY) {
                shmp->chat_history[shmp->history_cnt] = msg;
            } else {
                int i = shmp->history_cnt % MAX_HISTORY; 
                shmp->chat_history[i] = msg;
            }
            shmp->history_cnt++;


            sem_post(sem_cli);
            sem_post(sem_srv);
        }
    }

    pthread_cancel(console_TID);
    pthread_join(console_TID, NULL);

    munmap(shmp, sizeof(ChatSHM));
    sem_close(sem_srv);
    sem_close(sem_srv_cnt);
    sem_close(sem_cli);

    printf("[СЕРВЕР] Завершение работы.\n");
    return 0;
}