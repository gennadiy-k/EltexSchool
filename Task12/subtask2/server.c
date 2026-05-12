#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

int main() {
    
    char buf;
    char *string = "Hi!";
    int status;
    pid_t cpid;
    if(mkfifo("fifo1", 0666) == -1) {
        if(errno != EEXIST) {
            perror("Ошибка:");
            exit(EXIT_FAILURE);
        }
    }
    cpid = fork();
    if(cpid == 0) {
        execl("./client", "client", NULL);
        if (errno == ENOENT) {
            perror("Ошибка: файл клиента не найден");
            _exit(EXIT_FAILURE);
        }
        else {
            perror("Ошибка exec");
            _exit(EXIT_FAILURE);
        }
    }
    else {
        int fd = open("fifo1", O_WRONLY);
        write(fd, string, strlen(string)+1);
        close(fd);
        wait(&status);
        unlink("fifo1");
        exit(EXIT_SUCCESS);
    }
    return 0;
}