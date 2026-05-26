#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

int main(int argc, char* argv[]) {

    if (argc < 2 || argc > 2) {
        printf("Ошибка: не введен PID процесса.\n");
        printf("В командную строку вводить: ./kill <PID>\n");
        exit(1);
    }

    pid_t PID = (pid_t)atoi(argv[1]);

    if (kill(PID, SIGUSR1) == -1) {
        perror("Не удалось отправить сигнал");
        exit(EXIT_FAILURE);
    }

    return 0;
}