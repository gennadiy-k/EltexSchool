#ifndef headers
#define headers

#include "pthread.h"

#define M 5 // кол-во ларьков
#define N 3 // кол-во потоков

/*Объявление глобальных переменных*/
extern int mag[M]; 
extern int demand[N];
extern int flag;
extern pthread_mutex_t mut[M];

/*Прототипы функций*/
void* thread_loader(void* args);
void* calc_demand(void* args);

#endif