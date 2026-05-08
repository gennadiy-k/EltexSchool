#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

int main() {
    
    int pipefd[2];
    pid_t cpid;
    int status;
    char buf;
    char *string = "Hi!";

    if(pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    if(cpid == 0) {
        close(pipefd[1]);
        while(read(pipefd[0], &buf, 1) > 0)
            write(STDOUT_FILENO, &buf, 1);
        write(STDOUT_FILENO, "\n", 1);
        close(pipefd[0]);
        _exit(EXIT_SUCCESS);
    }
    else {
        close(pipefd[0]);
        write(pipefd[1], string, strlen(string));
        close(pipefd[1]);
        wait(&status);
        exit(EXIT_SUCCESS);
    }
    return 0;
}