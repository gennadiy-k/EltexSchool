#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int main() {
    char buf[50];
    char msg[50];
    int fd = open("fifo1", O_RDONLY);
    ssize_t bytes = read(fd, buf, sizeof(buf)-1);
    msg[bytes] = '\0';
    int len = sprintf(msg, "%s", buf);
    printf("%s\n", msg);
    close(fd);
    return 0;
}