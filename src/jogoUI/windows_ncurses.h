
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
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#include "../common/common_data_structs.h"
#include "../common/constants.h"
#include "../common/messages.h"
#include "../common/utils.h"

#define SPACING_FACTOR 2

// Menu inicial
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 3

void terminate(int exitcode);
void saveUserInfo(SignUpMessage *ptr);
void destroy_win(WINDOW *local_win);

void mostraMapa(WINDOW *mapawin, int height, int width, Character* player, Map map);
void pedras(WINDOW *win, int nRocks);
void bloqueios(WINDOW *win, int nMBlocks);
void nivel(WINDOW *win, int level);
//void jogadores(WINDOW *win, char *playersCommaSeparated);

void executeCommand(char *command,WINDOW *win);
char* comandos(WINDOW * comandwin);
void comandos2(WINDOW * comandwin);
WINDOW* processCommand(char *command, WINDOW *window, int generalPipe, const char *senderUsername);


void drawCharacter(WINDOW *win, Character character);
void clearCharacter(WINDOW *win, Character character);
void moveCharacter(WINDOW *win, Character *character, int dx, int dy);
void destroyCharacter(Character *character);

//int validatePlayerName(Player activePlayers[], int numActivePlayers, char *name);
//void listBots(Game *game);



typedef struct {
    WINDOW * mapawin;
    WINDOW * nivelwin;
    WINDOW * jogadoreswin;
    WINDOW * bloqueioswin;
    WINDOW * pedraswin;
    WINDOW * Commandwin;
    WINDOW * notificationwin;
} Windows;


#endif //SO_TP_WINDOWS_NCURSES_H
