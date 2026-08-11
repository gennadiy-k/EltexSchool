#include <stdio.h>

#define N 10

int main() {

    int A[N], k=1;

    printf("Элементы массива А: ");
    for (int i = 0; i < N; i++) { 
        *(A+i) = k++; 
        printf("%d ", *(A+i));
    }
    printf("\n");
    return 0;
}