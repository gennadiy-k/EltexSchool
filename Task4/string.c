#include "headers_func.h"
#include <stdio.h>
#include <string.h>
#define N 10

/* Фукнция add_list заполняет поля в массиве структур list */
void add_list(struct abonent* list, int* cnt) {
    printf("Имя:\n");
    fgets((list+(*cnt))->name, N, stdin);
    (list+(*cnt))->name[strcspn((list+(*cnt))->name, "\n")] = 0; //для последующего вывода данных абонента одной строкой

    printf("Фамилия:\n");
    fgets((list+(*cnt))->second_name, N, stdin);
    (list+(*cnt))->second_name[strcspn((list+(*cnt))->second_name, "\n")] = 0; //для последующего вывода данных абонента одной строкой

    printf("Номер телефона:\n");
    fgets((list+(*cnt))->tel, N, stdin);
    (list+(*cnt))->tel[strcspn((list+(*cnt))->tel, "\n")] = 0; //для последующего вывода данных абонента одной строкой
    (*cnt)++;
}

/* Фукнция delete_list удаляет запись в массиве структур list */
void delete_list(struct abonent* list) {
    int num = 0;
    printf("Введите номер абонента для удаления:\n");
    scanf("%d", &num);
    while (getchar() != '\n'); //очистка буфера
    for (int i = num - 1; i < num; i++) {
        for (int j = 0; j < N; j++) {
            (list+i)->name[j] = '\0';
            (list+i)->second_name[j] = '\0';
            (list+i)->tel[j] = '\0';
        }
    }
    printf("Абонент №%d удален.\n", num);
}

/* Фукнция find_list осуществляет вывод на экран одинковых записей по полю name в массиве структур list */
void find_list(struct abonent* list, int user_cnt) {
    char name_cmp[10];
    int f = 0;
    printf("Введите имя для поиска: \n");
    fgets(name_cmp, 10, stdin);
    name_cmp[strcspn(name_cmp, "\n")] = 0; //удаляем \n
    for (int i = 0; i < user_cnt; i++) {
        if (strcmp(name_cmp, (list+i)->name) == 0) {
            printf("%3d) Имя: %s, Фамилия: %s, Номер телефона: %s\n", i+1, (list+i)->name, (list+i)->second_name, (list+i)->tel);
            f++;
        }
    }
    if (f == 0)
        printf("Сотрудник не найден.\n");
}

/* Фукнция print_list выводит все записи массива структур list */
void print_list(struct abonent* list, int user_cnt) {
    for (int i = 0; i < user_cnt; i++) {
        printf("%3d) Имя: %s, Фамилия: %s, Номер телефона: %s\n", i+1, (list+i)->name, (list+i)->second_name, (list+i)->tel);
    }
}
