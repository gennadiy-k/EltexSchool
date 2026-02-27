#ifndef headers_func
#define headers_func

/* Опрределние структуры abonent */
struct abonent {
    char name[10];
    char second_name[10];
    char tel[10];
};

struct Node {
    struct abonent data;
    struct Node* next;
    struct Node* prev;
};

/* Прототипы пользовательских функций */
void fill_arr(char*, char*, char*);
struct Node* create_node(char*, char*, char*);
void add_node(struct Node**, struct Node**, char*, char*, char*);
void delete_list(struct Node**, struct Node**);
void find_list(struct Node*);
void print_list(struct Node*);
#endif