#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include "/home/hugoj7/Secretária/TP SO/SO-TP/src/motor/data_structs.h"

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

        char *token = strtok(command, " ");
        char *targetName = strtok(NULL, " ");
        char *message = strtok(NULL, "");

        if (targetName != NULL && message != NULL)  {
            /*
           // Validar se o nome do destinatário existe
            if (validatePlayerName(game->players, game->nPlayers, targetName)) {
                mvprintw(12, 0, "Mensagem enviada para %s: %s", targetName, message);
            } else {
                mvprintw(12, 0, "Erro: Utilizador %s não encontrado.", targetName);
            }*/
            executeCommand(command,&centeredWin, height, width);
        }
        else
        {
            executeCommand("msg <nome_utilizador> <mensagem>",&centeredWin, height, width);
        }


    }

    else
    if (strcmp(command, "exit") == 0) {
        endwin();
        exit(0);
    }

    else if (strcmp(command, "users") == 0) {
        /*
        printf("Lista de Jogadores Ativos:\n");
        for (int i = 0; i < game->nPlayers; i++) {
            printf("- %s\n", game->players[i].name);
        }*/
        executeCommand(command,&centeredWin, height, width);
    }

    else if (strncmp(command, "kick", 4) == 0) {

    char *token = strtok(command, " ");
    char *targetName = strtok(NULL, " ");
    char *aux;

    if (targetName != NULL) {

        aux = (char *)malloc(strlen(targetName) + strlen(" banido") + 1);
        strcpy(aux,targetName);
        strcat(aux," banido");
        executeCommand(aux,&centeredWin, height, width);

        /*
    // Encontrar o jogador
    int playerIndex = -1;
    for (int i = 0; i < game->nPlayers; i++) {
    if (strcmp(targetName, game->players[i].name) == 0) {
    playerIndex = i;
    break;
    }
    }

    if (playerIndex != -1) {*/
    // Informar o jogador
        /*
    mvprintw(12, 0, "Jogador %s foi banido.", targetName);
    // Terminar o programa jogoUI do jogador
    //endwin();
    exit(0);*/
        free(aux);
    }
    else {
    //mvprintw(12, 0, "Erro: Utilizador %s não encontrado.", targetName);

        executeCommand("kick <nome do jogador>",&centeredWin, height, width);
    }



    }

    else if (strcmp(command, "bots") == 0) {
        //listBots(game);
        executeCommand(command,&centeredWin, height, width);
    }

    else if (strcmp(command, "bmov") == 0) {
        executeCommand(command,&centeredWin, height, width);
    }

    else if (strcmp(command, "rbm") == 0) {
        executeCommand(command,&centeredWin, height, width);
    }

    else if (strcmp(command, "begin") == 0) {
        executeCommand(command,&centeredWin, height, width);
    }

    else if (strcmp(command, "end") == 0) {
        executeCommand(command,&centeredWin, height, width);
    }

    else
    {
        executeCommand("invalido",&centeredWin, height, width);
    }


}

/*
void listBots(Game *game) {
    if (game->nBots == 0) {
        mvprintw(18, 0, "Não há bots ativos no momento.");
    } else {
        mvprintw(18, 0, "Lista de Bots Ativos:\n");
        for (int i = 0; i < game->nBots; i++) {
            mvprintw(19 + i, 0, "- %s", game->bots[i].name);
        }
    }
}*/

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

