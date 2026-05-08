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
    pid_t cpid1, cpid2;
    int pipefd[2];
    int flag;

    while(1){
        flag = 0;
        /*Выводим текущую директорию*/
        if(getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s> ", cwd);
        } else {
            perror("getcwd error");
        }
        fflush(NULL);

        /*Читаем строку из потока*/
        if(fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        input[strcspn(input, "\n")] = '\0';

        if(strlen(input) == 0) {
            continue;
        }

        /*Парсинг строки*/
        int i = 0;
        input_parser[i] = strtok(input, " ");
        while(*(input_parser+i) != NULL && i < 19) {
            i++;
            *(input_parser+i) = strtok(NULL, " ");
        }
        /*2-ой парсинг строки для символа |*/
        int pipe_index = -1; // Индекс для разделения левой и правой частей (перед и после |)
        for(int j = 0; j < i; j++) {
            if(strcmp(*(input_parser+j),"|") == 0) {
                flag = 1; // Флаг для создания канала и второго дочернего процесса
                pipe_index = j;
                *(input_parser+j) = NULL;
                break;
            }
        }
        /*Проверка для команды cd*/
        if (input_parser[0] != NULL && strcmp(input_parser[0], "cd") == 0) {
            if (input_parser[1] == NULL) { // Если cd без аргументов, то переходим корень
                if (chdir("/") != 0) {
                    perror("cd error");
                }
            } 
            else {
                if (chdir(input_parser[1]) != 0) {
                    perror("cd error"); // Если папки не существует
                }
            }
        continue; 
        }
        /*Проверка для команды exit*/
        if(strcmp(*(input_parser), "exit") == 0) {
            break;
        }
        /*Создаем неимен. канал*/
        if(flag){
            if(pipe(pipefd) == -1) {
                perror("pipe error");
                exit(EXIT_FAILURE);
            }
        }
        /*Порождаем дочерние процессы*/
        cpid1 = fork();
        if(cpid1 == 0) {
            if(flag){
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }
            if(execvp(*(input_parser), input_parser) == -1) {
                perror("Command not found");
                exit(EXIT_FAILURE);
            }
        }
        if(flag) {
            cpid2 = fork();
            if(cpid2 == 0) {
                close(pipefd[1]);
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[0]);
                if(execvp(*(input_parser + pipe_index + 1), input_parser + pipe_index + 1) == -1) {
                    perror("Command not found");
                    exit(EXIT_FAILURE);
                }
            }
        }
        if(flag) {
            close(pipefd[0]);
            close(pipefd[1]);
        }
        while(wait(&status) > 0);
    
    }
    return 0;
}