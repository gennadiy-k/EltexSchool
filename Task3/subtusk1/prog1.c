#include <stdio.h>

int main() {
    
    unsigned char *ptr;
    int A, N = 1, num;

    printf("Введите число А:");
    scanf("%d", &A);
    ptr = (char *)&A;
    printf("16-e представление числа А: %X\n", A);
    
    while(N) {
        printf("Введите число для изменения 3-го байта [0...255]: ");
        scanf("%d", &num);
        if (num >= 0 && num <= 255) {
            N = 0;
            *(ptr+2) = num; // Выражение для изменения 3-го байта
            printf("16-e представление числа А после изменения: %X\n", A);
            for (int i = 1; i <= sizeof(A); i++){
                printf("%d байт числа А (dec/hex): %3d / %2X\n", i, *ptr, *ptr);
                ptr++;
            }
        }
        else
            printf("Число вне диапазона");
    }
    return 0;
}