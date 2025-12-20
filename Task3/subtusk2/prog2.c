#include <stdio.h>
int main(void)
{
    float x = 5.0;
    printf("x = %f, ", x);
    float y = 6.0;
    printf("y = %f\n", y);
    float *xp = &y; // "y" хранит нужное значение для получения результата 12.0. Теперь указатель "xp" хранит адрес переменной "y"
    float *yp = &y;
    printf("Результат: %f\n", *xp + *yp);
    return 0;
}
