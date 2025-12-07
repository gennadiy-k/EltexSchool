#include <stdio.h>

#define N 5

int main() {
    int B[N], C; //С - переменная для хранения элементов массива
    printf("Введите элементы массива B:");
    for (int i=0; i < N; i++) {
        scanf("%d", &B[i]);
    }
    for (int i = 0, j = N-1; i <= j; i++, j--) {
        C = B[i]; 
        B[i] = B[j];
        B[j] = C;
    }
    printf("Элементы массива B после перестановки: ");
    for (int i = 0; i < N; i++) {
        printf("%d", B[i]);
    }
    return 0;
}