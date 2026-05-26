#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int main(void) {
    sigset_t set;
    int ret;

    // Инициализируем маску сигналов
    sigemptyset(&set);
    
    // Добавляем SIGINT в маску 
    sigaddset(&set, SIGINT);

    // Блокируем сигнал SIGINT 
    ret = sigprocmask(SIG_BLOCK, &set, NULL); 
    if (ret < 0) {
        perror("Ошибка sigprocmask\n");
        exit(EXIT_FAILURE);
    }

    printf("Программа запущена. PID: %d\n", getpid());
    printf("Отправьте сигнал командой: kill -2 %d или ./sigsnd %d\n", getpid(), getpid());

    while (1) {
        sleep(1);
    }

    exit(EXIT_SUCCESS);
}
