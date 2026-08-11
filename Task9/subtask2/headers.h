#ifndef headers
#define headers

#ifndef PATH_MAX
#define PATH_MAX 4096 /* Вручную определям макс. длину пути к файлам */
#endif

/* Определение структуры для 2 панелей */
struct panel {
    WINDOW *win;
    int n;
    int selected; //Подвсветка выбранной строки
    char path[PATH_MAX];
    struct dirent **namelist;
    int scroll_top; //Индекс первого видимого файла
};


/* Инициализация curses */
void init_curses();
/* Функция перезагрузки списка файлов */
void refresh_panel(struct panel*);
/* Функция для отрисовки панели */
void draw_panel(struct panel*, int);
/* Функция для отрисовки нижней панели меню */
void draw_menubar(WINDOW* menubar);
#endif