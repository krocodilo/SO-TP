#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include "../motor/data_structs.h"

#define MAX_NAME_LENGTH 50
#define MAX_COMMAND_LENGTH 50
#define MAX_ACTIVE_PLAYERS 5

void processCommand(Game *game, char *command);
//int validatePlayerName(Player activePlayers[], int numActivePlayers, char *name);
void listBots(Game *game);


void executeCommand(char *command,WINDOW **win, int height, int width) {
    initscr();
    echo();
    cbreak();

    int yMax, xMax,a;
    getmaxyx(stdscr, yMax, xMax);

    int startY = (yMax - height) / 2;
    int startX = (xMax - width) / 2;

    *win = newwin(height, width, startY, startX);
    box(*win,0,0);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_color(COLOR_CYAN,0-999,0-999,0-999);

    wattron(*win,COLOR_PAIR(1));
    mvwprintw(*win, 1,1,"Notificacao de Comando!");
    mvwprintw(*win, 4,1,"%s",command);
    wattroff(*win,COLOR_PAIR(1));

    noecho();
    nocbreak();
    refresh();
    wrefresh(*win);
}


char* comandos(void){

    char comando[MAX_COMMAND_LENGTH];
    initscr();
    echo();
    cbreak();

    int yMax, xMax,a;
    getmaxyx(stdscr, yMax, xMax);

    WINDOW * comandwin = newwin(3,xMax-2, yMax-3,1);
    box(comandwin,0,0);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_color(COLOR_CYAN,0-999,0-999,0-999);

    wattron(comandwin,COLOR_PAIR(1));
    mvwprintw(comandwin, 1,1,"Comandos: ");
    wattroff(comandwin,COLOR_PAIR(1));

    wgetstr(comandwin,comando);

    noecho();
    nocbreak();
    refresh();
    wrefresh(comandwin);

    return strdup(comando);
}

/*int validatePlayerName(Player activePlayers[], int numActivePlayers, char *name) {

   for (int i = 0; i < numActivePlayers; ++i) {
       if (strcmp(name, activePlayers[i].name) == 0) {
           return 1;
       }
   }
   return 0;

}*/


// Função para processar comandos do jogador
void processCommand(Game *game, char *command) {

    WINDOW *centeredWin;
    int height = 10;
    int width = 50;

    if (strcmp(command, "players") == 0) {
        /*
        printf("Lista de Jogadores:\n");
        for (int i = 0; i < game->nPlayers; i++) {
            printf("- %s\n", game->players[i].name);
        }*/
        executeCommand(command,&centeredWin, height, width);
    }else

    if (strncmp(command, "msg", 3) == 0) {
        char* cmdCopy = strdup(command);    // apenas para meta 1


        char *cmd = strtok(command, " ");
        char *targetName = strtok(NULL, " ");
        char *message = strtok(NULL, "");

        if (targetName != NULL && message != NULL) {
            /*
           // Validar se o nome do destinatário existe
            if (validatePlayerName(game->players, game->nPlayers, targetName)) {
                mvprintw(12, 0, "Mensagem enviada para %s: %s", targetName, message);
            } else {
                mvprintw(12, 0, "Erro: Utilizador %s não encontrado.", targetName);
            }*/
            executeCommand(cmdCopy,&centeredWin, height, width);
        }
        else
        {
            executeCommand("msg <nome_utilizador> <mensagem>",&centeredWin, height, width);
        }


    }

    else
    if (strcmp(command, "exit") == 0) {
        // TODO: avisar motor e sair ordeiramente
        endwin();
        exit(0);
    }


    else
    {
        executeCommand("Comando invalido!",&centeredWin, height, width);
    }


}

int main(int argc, char *argv[]) {
    Player activePlayers[MAX_ACTIVE_PLAYERS];
    char *command;

    Game game;
    char playerName[MAX_NAME_LENGTH];


    if (argc < 2) {
        printf("Nome do jogador como argumento.\n");
        return 1;
    }

    strncpy(playerName, argv[1], MAX_NAME_LENGTH - 1);
    playerName[MAX_NAME_LENGTH - 1] = '\0';

    mvprintw(5, 0, "Jogador %s criado!", playerName);

    int exitRequested = 0;

    while (!exitRequested) {
        char *command = comandos();  // Alocar memória para a string de comando
        processCommand(&game, command);  // Processar o comando
        free(command);  // Liberar a memória alocada para o comando

        // Verificar se o comando é "exit" ou outro comando que solicita a saída do loop
        if (strcmp(command, "exit") == 0) {
            exitRequested = 1;  // Sair do loop
        }
    }


    getch();
    endwin();

    return 0;
}

