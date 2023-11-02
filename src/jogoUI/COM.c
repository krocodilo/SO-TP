#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#define MAX_NAME_LENGTH 50
#define MAX_COMMAND_LENGTH 50

int main(int argc, char *argv[]) {
    initscr();
    echo();
    cbreak();

    char comando[30];

    int yMax, xMax,a;
    getmaxyx(stdscr, yMax, xMax);

    WINDOW * inputwin = newwin(3,xMax-2, yMax-3,1);
    box(inputwin,0,0);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_color(COLOR_CYAN,0-999,0-999,0-999);

    wattron(inputwin,COLOR_PAIR(1));
    mvwprintw(inputwin, 1,1,"Comandos: ");
    wattroff(inputwin,COLOR_PAIR(1));

    wgetstr(inputwin,comando);

    noecho();
    nocbreak();
    refresh();
    wrefresh(inputwin);

}