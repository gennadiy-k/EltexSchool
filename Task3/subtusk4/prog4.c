#include <stdio.h>

#define N 99

// Функция для поиска подростки в строке
char* string_compare(char *string, char *pattern) {

    int strlen_str = 0, strlen_P = 0; // Переменные для вычисления длины строки и подстроки
    char *ptr_str; // Указатель на начало подстроки в строке

    while (*(string + strlen_str) != '\0') { // Вычисляем длину строки с учетом '\n'
        strlen_str++;
    }
    while (*(pattern + strlen_P) != '\0') { // Вычисляем длину подстроки с учетом '\n'
        strlen_P++;
    }
    if (strlen_P > strlen_str) {
        ptr_str = NULL; // Если подстрока длиннее строки
        return ptr_str; 
    }

    printf("Длина строки: %d\n", strlen_str);
    printf("Длина подстроки: %d\n", strlen_P);

    for (int i = 0; i < strlen_str - 1; i++) {
        for (int j = 0; j < strlen_P - 1 && *(string+i+j) == *(pattern+j); j++) { 
            if (j == strlen_P - 2) { // strlen_P-2, т.к. нумерация элементов с 0 и подстрока оканчивается "\n"
                ptr_str = string + i; //Указатель на начало найденной подстроки в строке
                return ptr_str;
            }
        }   
    }
    return NULL; // Если цикл закончился и не нашел совпадений
}

int main() {

    char str[N], P[N]; // str - строка, P - подстрока
    char *ptr_str_res; // Указатель на начало подстроки в строке

    printf("Введите строку:");
    fgets(str, sizeof(str), stdin);
    printf("Введите искомую подстроку:");
    fgets(P, sizeof(P), stdin);
    
    printf("Адрес начала строки: %p\n", str);

    ptr_str_res = string_compare(str, P); // Возвращаем указатель на начало подстроки в строке или NULL

    if (ptr_str_res == NULL)
        printf("Совпадений не найдено. Результат: %p\n", ptr_str_res);
    else 
        printf("Адрес начала подстроки в строке: %p\n", ptr_str_res);

    return 0;
}
