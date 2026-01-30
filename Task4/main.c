#include "headers_func.h"
#include <stdio.h>
#define user_cnt 100

int main() {
    int flag = 1;
    int cnt = 0;
    int sw;

    struct abonent list[user_cnt] = {0};

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
                if(cnt < user_cnt){ //условие для контроля числа абонентов
                    add_list(list, &cnt);
                }
                else {
                    printf("Справочник переполнен.\n");
                }
                continue;
            case 2:
                delete_list(list);
                break;
            case 3:
                find_list(list, user_cnt);
                break;
            case 4:
                print_list(list, user_cnt);
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