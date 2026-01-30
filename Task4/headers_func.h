#ifndef headers_func
#define headers_func

/* Опрределние структуры abonent */
struct abonent {
    char name[10];
    char second_name[10];
    char tel[10];
};

/* Прототипы пользовательских функций */
void add_list(struct abonent*, int*);
void delete_list(struct abonent*);
void find_list(struct abonent*, int);
void print_list(struct abonent*, int);
#endif