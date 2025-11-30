#include <stdio.h>

int main() {
    int num1, num2, num3; //num3 - переменная для числа с измененным третьим байтом
    int N = 1; //для выхода из цикла while
    printf("Введите число: ");
    scanf("%d", &num1);
    printf("16-e представление числа %d: %x \n", num1, num1);
    while (N) {
        printf("Введите число для изменения 3-го байта [0...255]: ");
        scanf("%d", &num2);
        if (num2 >= 0 && num2 <= 255) {
            N = 0;
            num3 = (num1 & (0xFF00FFFF) | (num2 << 16));
            printf("Результат замены третьего байта в числе %d: %x \n", num1, num3);
            printf("Десятичное представление числа %x: %d \n", num3, num3);
        }
        else {
            printf("Число вне диапазона");
        }
    }
    return 0;
}