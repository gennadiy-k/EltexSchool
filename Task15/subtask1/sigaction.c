#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

// Обработчик сигналов
void sig_handler(int sig_num, siginfo_t *info, void *args) {

    int *param = (int *)args; 
    
    printf("Signal SIGUSR1! - %d %d %d\n", sig_num, *param, info->si_signo);

}

int main(void) {
    struct sigaction handler;
    sigset_t set;
    int ret;

    // Инициализируем маску сигналов
    sigemptyset(&set);
    
    // Добавляем SIGUSR1 в маску 
    sigaddset(&set, SIGUSR1);

    // Настраиваем структуру sigaction
    handler.sa_sigaction = sig_handler;
    handler.sa_mask = set;
    

    // Регистрируем обработчик для сигнала SIGUSR1
    ret = sigaction(SIGUSR1, &handler, NULL); 
    if (ret < 0) {
        perror("Ошибка устаноуки обработчика сигнала!\n");
        exit(EXIT_FAILURE);
    }

    printf("Программа запущена. PID: %d\n", getpid());
    printf("Отправьте сигнал командой: kill -10 %d или ./sigsnd %d\n", getpid(), getpid());

    while (1) {
        sleep(1);
    }

    exit(EXIT_SUCCESS);
}
