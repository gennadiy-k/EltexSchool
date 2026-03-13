#include "math_ops.h"
#include <stdio.h>

int main() {

    int flag = 1;
    int sw, var1, var2;

    while (flag) {

        printf("Меню:\n");
        printf("1) Сложение\n");
        printf("2) Вычитание\n");
        printf("3) Умножение\n");
        printf("4) Деление\n");
        printf("5) Выход\n");
        printf("Выберите пункт меню:");
        scanf("%d", &sw);

        switch (sw) {
            case 1:
                printf("Введите число: ");
                scanf("%d", &var1);
                printf("Введите число: ");
                scanf("%d", &var2);
                printf("Результат сложения: %d\n", add(var1, var2));
                break;
            case 2:
                printf("Введите число: ");
                scanf("%d", &var1);
                printf("Введите число: ");
                scanf("%d", &var2);
                printf("Результат вычитания: %d\n", sub(var1, var2));
                break;
            case 3:
                printf("Введите число: ");
                scanf("%d", &var1);
                printf("Введите число: ");
                scanf("%d", &var2);
                printf("Результат умножения: %d\n", mul(var1, var2));
                break;
            case 4:
                printf("Введите число: ");
                scanf("%d", &var1);
                printf("Введите число: ");
                scanf("%d", &var2);
                printf("Результат деления: %d\n", div(var1, var2));
                break;
            case 5:
                flag = 0;
                printf("Завершение работы\n");
                break;
            default:
                printf("Выбран некорректный пункт меню\n");
                break;
        }
    }
    
    return 0;
}
