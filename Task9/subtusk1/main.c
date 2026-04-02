#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    char text[] = "String from file";
    int fd = open("output.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Ошибка при открытии файла");
        exit(EXIT_FAILURE);
    }
    ssize_t nr = write(fd, text, strlen(text));
    // printf("%zu\n", strlen(text));
    if (nr == -1) {
        perror("Ошибка при записи в файл");
        exit(EXIT_FAILURE);
    }
    off_t ret = lseek(fd, -strlen(text), SEEK_CUR);
    if (ret == -1) {
        perror("Ошибка вызова lseek");
        exit(EXIT_FAILURE);
    }
    char buf[50];
    ssize_t n = read(fd, buf, sizeof(buf)-1);
    buf[n] = '\0';
    // printf("%zu", n);
    if (n > 0) {
        printf("Строка из файла: %s\n", buf);
    }
    else if (n == 0) {
        printf("Файл пуст");
    }
    else {
        perror("Ошибка чтения");
        exit(EXIT_FAILURE);
    }
    close(fd);
    return 0;
}