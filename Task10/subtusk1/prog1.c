#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
    
    pid_t child_pid;
    int status;
    
    child_pid = fork();
    if(child_pid == 0) {
        printf("Child pid: %d\n", getpid());
        printf("Parent pid(main): %d\n", getppid());
        exit(1);
    }
    else {
        printf("Parent pid: %d\n", getpid());
        printf("Parent pid(bash): %d\n", getppid());
        wait(&status);
        printf("Status: %d\n", WEXITSTATUS(status));
    }
    return 0;
}