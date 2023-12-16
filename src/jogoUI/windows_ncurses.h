
#ifndef SO_TP_WINDOWS_NCURSES_H
#define SO_TP_WINDOWS_NCURSES_H

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include "../motor/data_structs.h"
#include <sys/types.h>
#include <sys/stat.h>

//#define FIFO_FILE_IN "fifoIN"
//#define FIFO_FILE_OUT "fifoOUT"

#define MAX_COMMAND_LENGTH 50
#define MAX_ACTIVE_PLAYERS 5
#define SPACING_FACTOR 2

//menu inicial
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 3


int showMenu(char **choices, int n_choices, int *highlight);
int runMenuLogic();
void destroy_win(WINDOW *local_win);
void mostraMapa(WINDOW *mapawin, int height, int width, Character* player);
void pedras(WINDOW *win, int height, int width,Game *game);
void bloqueios(WINDOW *win, int height, int width,Game *game);
void nivel(WINDOW *win, int height, int width,Game *game);
void jogadores(WINDOW *win, int height, int width,Game *game);
void executeCommand(char *command,WINDOW *win, int height, int width,Game *game);
char* comandos(WINDOW * comandwin);
void comandos2(WINDOW * comandwin);
WINDOW* processCommand(Game *game, char *command,WINDOW *centeredWin);
void drawCharacter(WINDOW *win, Character character);
void clearCharacter(WINDOW *win, Character character);
void moveCharacter(WINDOW *win, Character *character, int dx, int dy);
void destroyCharacter(Character *character);

Character initCharacter(int x, int y, char symbol);
int isLinhaEmBranco(const char *linha);
void lerMapa(Game  *game);

//int validatePlayerName(Player activePlayers[], int numActivePlayers, char *name);
//void listBots(Game *game);





#endif //SO_TP_WINDOWS_NCURSES_H
