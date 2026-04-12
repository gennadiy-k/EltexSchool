#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
    
    pid_t pid1, pid2, pid3, pid4, pid5;
    int status;

    pid1 = fork();
    if(pid1 == 0) {
        printf("Child pid1: %d, parent pid(main): %d\n", getpid(), getppid());
        pid3 = fork();
        if(pid3 == 0){
            printf("Child pid3: %d, parent pid(pid1): %d\n", getpid(), getppid());
            exit(0);
        }
        wait(NULL);
        pid4 = fork();
        if(pid4 == 0){
            printf("Child pid4: %d, parent pid(pid1): %d\n", getpid(), getppid());
            exit(0);
        }
        while (wait(NULL) > 0); 
        exit(0);
    }
    pid2 = fork();
    if(pid2 == 0) {
        printf("Child pid2: %d, parent pid(main): %d\n", getpid(), getppid());
        pid5 = fork();
        if(pid5 == 0){
            printf("Child pid5: %d, parent pid(pid2): %d\n", getpid(), getppid());
            exit(0);
        }
        wait(NULL);
        exit(0);
    }

    printf("Parent pid(main): %d, parent pid(bash): %d\n", getpid(), getppid());
    while(wait(NULL) > 0);

    return 0;
}
