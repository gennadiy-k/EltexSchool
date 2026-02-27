#include "headers_func.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#define N 10

/* Фукнция create_node создает узел и заполняет поля структуры*/
struct Node* create_node(char* nName, char* nSec_name, char* nTel) {
    struct Node* Node = (struct Node*)malloc(sizeof(struct Node));
    if(Node != NULL) {
        strncpy(Node->data.name, nName, sizeof(Node->data.name));
        // Node->data.name[sizeof(Node->data.name) - 1] = '\0'; //добавляем символ конца строки
        strncpy(Node->data.second_name, nSec_name, sizeof(Node->data.second_name));
        // Node->data.second_name[sizeof(Node->data.second_name) - 1] = '\0';
        strncpy(Node->data.tel, nTel, sizeof(Node->data.tel));
        // Node->data.tel[sizeof(Node->data.tel) - 1] = '\0';
        Node->next = NULL;
        Node->prev = NULL;
    }
    else { 
        printf("Ошибка памяти, справочник переполнен. Абонент не может быть добавлен.\n");
        return NULL;
    }
    return Node;
}

/* Функция для реализует вставку узла в конец двусвязного списка*/
void add_node(struct Node **head, struct Node **tail, char* nName, char* nSec_name, char* nTel) {
    struct Node* Node = create_node(nName, nSec_name, nTel);
    if(*head == NULL) {
        *head = Node;
        *tail = Node;
    }
    else {
        (*tail)->next = Node;
        Node->prev = *tail;
        *tail = Node;
    }
}
/* Фукнция fill_arr заполняет строки до создания узла для последующего заполнения соответсвующих полей массива структур*/
void fill_arr(char* Name_arr, char* Sec_name_arr, char* Phone_arr, int* cnt) {
    printf("Имя:\n");
    fgets(Name_arr, N, stdin);
    Name_arr[strcspn(Name_arr, "\n")] = 0; //для вывода данных абонента одной строкой

    printf("Фамилия:\n");
    fgets(Sec_name_arr, N, stdin);
    Sec_name_arr[strcspn(Sec_name_arr, "\n")] = 0; //для вывода данных абонента одной строкой

    printf("Номер телефона:\n");
    fgets(Phone_arr, N, stdin);
    Phone_arr[strcspn(Phone_arr, "\n")] = 0; //для вывода данных абонента одной строкой
    (*cnt)++;
}
/* Фукнция delete_list удаляет абонента целиком и */
void delete_list(struct Node** head, struct Node** tail) {
    struct Node* ptr = *head;
    struct Node* ptr2 = NULL;
    printf("Введите имя для удаления: \n");
    char name_cmp[10];
    char sec_name_cmp[10];
    fgets(name_cmp, 10, stdin);
    name_cmp[strcspn(name_cmp, "\n")] = 0; //удаляем \n (на всякий случай для одинакового сравнения)
    printf("Введите Фамилию для удаления абонента: \n");
    fgets(sec_name_cmp, 10, stdin);
    sec_name_cmp[strcspn(sec_name_cmp, "\n")] = 0;
    while(ptr != NULL) {
        ptr2 = ptr->next;//запоминаем что идет следом
        if(strcmp(name_cmp, ptr->data.name) == 0 && strcmp(sec_name_cmp, ptr->data.second_name) == 0) { 
            if(*head == ptr) {
                *head = ptr->next;
            }
            if(*tail == ptr) {
                *tail = ptr->prev;
            }
            if(ptr->prev != NULL) {
                ptr->prev->next = ptr->next;
            }
            if(ptr->next != NULL) {
                ptr->next->prev = ptr->prev;
            }
            free(ptr);
            printf("Абонент удален\n");
        }
        ptr = ptr2;
    }
}

/* Фукнция find_list осуществляет вывод на экран одинковых записей по полю name от конца к началу */
void find_list(struct Node* tail) {
    struct Node* ptr = tail;
    char name_cmp[10];
    int f = 0;
    int i = 0;
    printf("Введите имя для поиска: \n");
    fgets(name_cmp, 10, stdin);
    name_cmp[strcspn(name_cmp, "\n")] = 0; //удаляем \n (на всякий случай для одинакового сравнения)
    // while(ptr != NULL) {
    //     printf("%3d) Имя: %s, Фамилия: %s, Номер телефона: %s\n", i+1, (list+i)->name, (list+i)->second_name, (list+i)->tel);
    // }
    while(ptr != NULL) {
    i++;
        if (strcmp(name_cmp, ptr->data.name) == 0) {
            printf("%3d) Имя: %s, Фамилия: %s, Номер телефона: %s\n", i, ptr->data.name, ptr->data.second_name, ptr->data.tel);
        }
    f++;
    ptr = ptr->prev;
    }
    if (f == 0)
        printf("Сотрудник не найден.\n");
}

/* Фукнция print_list выводит все записи от начала к концу*/
void print_list(struct Node* head) {
    int i = 0;
    struct Node* ptr = head;
    while(ptr != NULL) {
        i++;
        printf("%3d) Имя: %s, Фамилия: %s, Номер телефона: %s\n", i, ptr->data.name, ptr->data.second_name, ptr->data.tel);
        ptr = ptr->next;
    }
}
