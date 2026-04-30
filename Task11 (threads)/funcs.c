#include "headers.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void* thread_loader(void* args) {
    while(flag) {
        int j = rand() % M; //rand() используется только для 1 потока погрузчика
        pthread_mutex_lock(&mut[j]);
        mag[j] += 200;
        printf("Поток-погрузчик добавил в ларек #%d 200 товаров\n", j+1);
        pthread_mutex_unlock(&mut[j]);
        sleep(2);
        if (!flag) break;
    }
    printf("Поток-погрузчик закончил работу.\n");
    return NULL;
}

void* calc_demand(void* args) {
    int* i = (int*)args; //номер потока (покупателя)
    printf("Поток-покупатель #%d проснулся. Текущее кол-во потребности %d\n", *i+1, demand[*i]);
    while(demand[*i] > 0) {
        for (int j = 0; j < M; j++) {
            pthread_mutex_lock(&mut[j]);
            printf("Поток-покупатель #%d с потребностью %d зашел в ларек #%d, кол-во товаров %d\n", *i+1, demand[*i], j+1, mag[j]);
            int can_take = (demand[*i] < mag[j]) ? demand[*i] : mag[j];
            demand[*i] -= can_take; // вычитаем у покупателя
            mag[j] -= can_take;    // вычитаем из магазина
            printf("У потока-покупателя #%d стало %d потребности\n", *i+1, demand[*i]);
            int current_mag = mag[j];//сохраняем состояние магазниа внутри критической секции 
            pthread_mutex_unlock(&mut[j]);
            if (current_mag <= 0 && demand[*i] > 0) {
                printf("Поток-покупатель #%d: в ларьке %d пусто. Уходит в сон на 2 сек\n", *i+1, j+1);
                sleep(2);
            }
            if (demand[*i] <= 0) {
                break;
            }
        }
    }
    printf("Покупатель #%d закончил закупку, потребность %d\n", *i+1, demand[*i]);
    return NULL;
}