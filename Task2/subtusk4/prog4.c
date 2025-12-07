#include <stdio.h>

#define N 5

int main() {
    int A[N][N] = {0}, num = 1, top = 0, bottom = N - 1, left = 0, right = N - 1;
    while (num <= N * N) {
        // Заполнение верхней строки
        for (int i = left; i <= right; i++) {
            A[top][i] = num++;
        }
        top++;

        // Заполнение правого столбца
        for (int i = top; i <= bottom; i++) {
            A[i][right] = num++;
        }
        right--;

        // Заполнение нижней строки
        if (top <= bottom) {
            for (int i = right; i >= left; i--) {
                A[bottom][i] = num++;
            }
            bottom--;
        }

        // Заполнение левого столбца
        if (left <= right) {
            for (int i = bottom; i >= top; i--) {
                A[i][left] = num++;
            }
            left++;
        }
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%2d ", A[i][j]); 
        }
        printf("\n");
    }

    return 0;
}