#define _DEFAULT_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <dirent.h>
#include "headers.h"
int main() {

    WINDOW *menubar;
    init_curses();
    refresh(); 
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    menubar = newwin(1, COLS, LINES - 1, 0);

    struct panel panels[2];
    for (int i = 0; i < 2; i++) {
        panels[i].win = newwin(rows - 1, cols / 2, 0, i * (cols / 2));
        realpath(".", panels[i].path); // Получаем абсолютный путь текущей папки
        panels[i].selected = 0;
        panels[i].namelist = NULL;
        panels[i].n = 0;
        panels[i].scroll_top = 0;
        refresh_panel(&panels[i]);
    }

    int side = 0; //side = 0 - лево, side = 1 - право
    int ch;
    int flag = 1;
    while (flag) {
        draw_menubar(menubar);
        draw_panel(&panels[0], side == 0);
        draw_panel(&panels[1], side == 1);
        ch = getch();
        if (ch == 'q') {
            break;
        }
        struct panel *ptr = &panels[side];
        int h, w;
        getmaxyx(panels[side].win, h, w); //Высота текущего окна
        int on_screen = h - 2;
        switch (ch) {
        case 9: // TAB
            side = !side;
            break;
        case KEY_UP:
            if (ptr->selected > 0) {
            ptr->selected--;
            if (ptr->selected < ptr->scroll_top) {
                ptr->scroll_top--;
            }
        }
            break;
        case KEY_DOWN:
            if (ptr->selected < ptr->n - 1) {
            ptr->selected++;
            if (ptr->selected >= ptr->scroll_top + on_screen) {
                ptr->scroll_top++;
            }
        }
            break;
        case 10: //ENTER
            if (ptr->n == 0) break;
            struct stat metadata;
            char next_path[PATH_MAX];
            snprintf(next_path, sizeof(next_path), "%s/%s", ptr->path, ptr->namelist[ptr->selected]->d_name);
            
            if(stat(next_path, &metadata) == 0 && S_ISDIR(metadata.st_mode)) {
                realpath(next_path, ptr->path);
                ptr->selected = 0;
                refresh_panel(ptr);
            }
            ptr->scroll_top = 0; 
            ptr->selected = 0;
            refresh_panel(ptr);
            break;
        case KEY_F(1):
            flag = 0;
        }
    }
    endwin();
    return 0;
}