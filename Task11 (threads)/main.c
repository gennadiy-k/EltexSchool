#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "headers.h"

int mag[M];  //кол-во товаров в ларьках
int demand[N]; //потребность покупателей
int flag = 1;
pthread_mutex_t mut[M]; //мьютексы соответсвующих ларьков

int main() {
    srand(time(NULL));
    int i = 0;
    int* status;
    pthread_t TID[N+1];//TID[3]-индентификатор потока погрузчика

    for (i = 0; i < M; i++) {
        if (pthread_mutex_init(&mut[i], NULL) != 0) {
            perror("Ошибка инициализации мьютекса");
            exit(1);
        }
    }

    for (i = 0; i < M; i++) {
        mag[i] = (rand() % 201) + 900; //определяем количество товаров в M ларьках
    }

    for (i = 0; i < N; i++) {
        demand[i] = (rand() % 201) + 9900; //определяем потребность для N покупателей
    }
    int idx[N];
    for (i = 0; i < N; i++) {
        idx[i] = i;
        pthread_create(&TID[i], NULL, calc_demand, (void*)&idx[i]);
    }

    pthread_create(&TID[N], NULL, thread_loader, (void*)&i);
    
    for (i = 0; i < N; i++) {
        pthread_join(TID[i], (void**)&status);
    }
    flag = 0;
    pthread_join(TID[N], (void**)&status);
    
    for (i = 0; i < M; i++) {
        pthread_mutex_destroy(&mut[i]);
    }
    return 0;
}

