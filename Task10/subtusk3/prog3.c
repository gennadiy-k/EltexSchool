#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
int main() {

    char input[100];
    char *input_parser[20];
    char cwd[4096];
    int status;
    pid_t pid;

    while(1){
    /*Выводим текущую директорию*/
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s> ", cwd);
    } else {
        perror("getcwd error");
    }
    fflush(NULL);
        if(fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
    input[strcspn(input, "\n")] = 0;
        if(strlen(input) == 0) {
            continue;
        }
    int i = 0;
    input_parser[i] = strtok(input, " ");
        while(*(input_parser+i) != NULL && i < 19) {
            i++;
            *(input_parser+i) = strtok(NULL, " ");
        }
        if (strcmp(*(input_parser), "exit") == 0) {
            break;
        }
    pid = fork();
        if(pid < 0) {
            perror("Fork error");
        }
        else if (pid == 0) {
            if(execvp(*(input_parser), input_parser) == -1) {
                perror("Command not found");
            }
        exit(1);
        }
        else {
            wait(&status);
        }
    
    }
    return 0;
}