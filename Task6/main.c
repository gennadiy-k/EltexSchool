#include "headers_func.h"
#include <stdio.h>
#define user_cnt 100

int main() {
    int flag = 1;
    int sw;

    struct Node *head = NULL;
    struct Node *tail = NULL;

    char Name[10], Second_name[10], Tel[10]; //для заполнения до создания узла
    while (flag) {
        sw = 0;
        printf("Меню:\n");
        printf("1) Добавить абонента\n");
        printf("2) Удалить абонента\n");
        printf("3) Поиск абонентов по имени\n");
        printf("4) Вывод всех записей\n");
        printf("5) Выход\n");
        printf("Выберите пункт меню: ");
        scanf("%d", &sw);
        while (getchar() != '\n'); //очистка буфера
        switch (sw) {
            case 1:
                fill_arr(Name, Second_name, Tel);
                add_node(&head, &tail, Name, Second_name, Tel);
                break;
            case 2:
                delete_list(&head, &tail);
                break;
            case 3:
                find_list(tail);
                break;
            case 4:
                print_list(head);
                break;
            case 5:
                flag = 0;
                printf("Завершение работы.\n");
                continue; //тут continue, чтобы не сработал default
            default:
                printf("Выбран некорректный пункт меню.\n");
                break;
        }
    }
    return 0;
}