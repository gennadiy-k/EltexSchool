#define _DEFAULT_SOURCE 
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include "headers.h"
#include <dirent.h>

void init_curses() {
    initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLUE);  // Основной фон
    init_pair(2, COLOR_BLACK, COLOR_CYAN);  // Цвет выделенной строки
    init_pair(3, COLOR_YELLOW, COLOR_BLUE);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);
    noecho();              
    curs_set(0);           
    keypad(stdscr, TRUE);  
    cbreak(); 
}

void refresh_panel(struct panel *p) {
    if (p->namelist != NULL) {
        for(int i = 0; i < p->n; i++) {
            free(p->namelist[i]);
        }
        free(p->namelist);
        p->namelist = NULL;
        p->n = 0;
    }
    p->n = scandir(p->path, &p->namelist, NULL, alphasort);
    if (p->selected >= p->n) {
        p->selected = (p->n > 0) ? p->n - 1 : 0;
    }
}   
void draw_panel(struct panel *p, int side) {
    int rows, cols;
    getmaxyx(p->win, rows, cols);
    int on_screen = rows - 2; //кол-во видимых строк на экране
    wbkgdset(p->win, COLOR_PAIR(1)); 
    werase(p->win);
    box(p->win, 0, 0);
    mvwprintw(p->win, 0, 2, " %s ", p->path);
    for(int i = 0; i < on_screen && (p->scroll_top + i) < p->n; i++) {
        int i_top = p->scroll_top + i;
        wmove(p->win, i + 1, 1);
        bool is_selected = side && i == p->selected; //для подсветки
        if(is_selected) {
            wattron(p->win, COLOR_PAIR(2));
        }
        else {
            wattron(p->win, COLOR_PAIR(1));
        }
        struct stat metadata;
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", p->path, p->namelist[i]->d_name);
        stat(full_path, &metadata);

        if (S_ISDIR(metadata.st_mode)) {
            waddch(p->win, '/');
        }
        else {
            waddch(p->win, ' ');
        }
        wprintw(p->win, "%-18.18s", p->namelist[i_top]->d_name);
        if(is_selected) {
            wattroff(p->win, COLOR_PAIR(2));
        } else {
            wattroff(p->win, COLOR_PAIR(1));
        }
    }
    wrefresh(p->win);
}
void draw_menubar(WINDOW *menubar) {
    wbkgd(menubar, COLOR_PAIR(4));
    wmove(menubar, 0, 1); 
    waddstr(menubar, "F1");
    wattron(menubar, COLOR_PAIR(2));
    waddstr(menubar, "Exit");
    wattroff(menubar, COLOR_PAIR(2));
    wmove(menubar, 0, 20);
    waddstr(menubar, "F2");
    wattron(menubar, COLOR_PAIR(2));
    waddstr(menubar, "Help");
    wattroff(menubar, COLOR_PAIR(2)); 
    wrefresh(menubar);
}
