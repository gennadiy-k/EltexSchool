#include <stdio.h>

int main() {
    int num;
    printf("Введите число: ");
    scanf("%d", &num);
    printf("Двоичное представление числа %d: ", num);
    for (int size = sizeof(num) * 8 - 1; size >= 0; size--) { //счетчик size = размер типа в битах - 1
        if (num & (1 << size)) //применяем побитовое И, используя побитовый сдвиг единицы влево от 0 до 31
            printf("1"); //если бит установлен
        else 
            printf("0");
    }
    printf("\n");
    return 0;
}