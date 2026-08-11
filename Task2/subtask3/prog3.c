#include <stdio.h>

#define N 3

int main() {
    int A[N][N];
    for (int i = 0; i < N; i++) { 
        for (int j = 0; j < N; j++) {
            A[i][j] = (j>((N-2-i))); //выражение для заполнения элементов матрицы 0 и 1
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
    return 0;
}