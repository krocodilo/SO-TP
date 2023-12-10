#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include "../motor/data_structs.h"


#define MAX_COMMAND_LENGTH 50
#define MAX_ACTIVE_PLAYERS 5
#define SPACING_FACTOR 2

//menu inicial
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 3

Map map;

int showMenu(char **choices, int n_choices, int *highlight);
int runMenuLogic();
WINDOW* processCommand(Game *game, char *command,WINDOW *centeredWin) ;
//int validatePlayerName(Player activePlayers[], int numActivePlayers, char *name);
void listBots(Game *game);
void destroy_win(WINDOW *local_win);


//Menu inicial

///////////////////////////////////////////////////////////////////////////

int showMenu(char **choices, int n_choices, int *highlight) {
    initscr();
    keypad(stdscr, TRUE);
    curs_set(0);

    int menu_height = 8;
    int menu_width = 20;
    int start_y = (LINES - menu_height) / 2;
    int start_x = (COLS - menu_width) / 2;


    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    WINDOW *menu_win = newwin(menu_height, menu_width, start_y, start_x);
    box(menu_win, 0, 0);
    wattron(menu_win,COLOR_PAIR(1));
    wrefresh(menu_win);

    int c;
    int op = 0;

    // Configura o timeout para 100 milissegundos (0.1 segundo)
    timeout(100);

    while (op != 1) {
        wclear(menu_win);
        box(menu_win, 0, 0);

        for (int i = 0; i < n_choices; ++i) {
            if (*highlight == i + 1) {
                wattron(menu_win, A_REVERSE);
                mvwprintw(menu_win, i + 1, 1, "%s", choices[i]);
                wattroff(menu_win, A_REVERSE);
            } else {
                mvwprintw(menu_win, i + 1, 1, "%s", choices[i]);
            }
        }

        wrefresh(menu_win);

        // Leitura de entrada com timeout
        c = getch();
        if (c != ERR) {
            switch (c) {
                case KEY_UP:
                    if (*highlight == 1)
                        *highlight = n_choices;
                    else
                        --(*highlight);
                    break;
                case KEY_DOWN:
                    if (*highlight == n_choices)
                        *highlight = 1;
                    else
                        ++(*highlight);
                    break;
                case 10:  // Enter key
                    op = 1;
                    break;
                default:
                    break;
            }
        }
    }
    wattroff(menu_win,COLOR_PAIR(1));
    werase(menu_win);
    wrefresh(menu_win);
    destroy_win(menu_win);
    endwin();
    keypad(stdscr, FALSE);
    return *highlight;
}

int runMenuLogic() {
    int op = 0;

    while (op != 1) {
        char *menuChoices[] = {"Jogar", "Opcoes", "Sobre", "Sair"};
        int n_menuChoices = sizeof(menuChoices) / sizeof(menuChoices[0]);
        int highlight = 1;

        int selectedOption = showMenu(menuChoices, n_menuChoices, &highlight);

        switch (selectedOption) {
            case 1:
                // Jogar
                op = 1;
                break;
            case 2:
                // Opcoes
                break;
            case 3:
                // Sobre
                break;
            case 4:
                // Sair
                exit(1);
        }
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////

void destroy_win(WINDOW *local_win){

    wborder (local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');

    wrefresh(local_win);
    delwin(local_win);
}

// Função para inicializar o personagem
Character initCharacter(int x, int y, char symbol) {
    Character character;
    character.x = x;
    character.y = y;
    character.symbol = symbol;
    return character;
}


// Função para desenhar o personagem na janela
void drawCharacter(WINDOW *win, Character character) {
    mvwprintw(win, character.y, character.x, "%c", character.symbol);
    wrefresh(win);
}

// Função para limpar o rastro do personagem na janela
void clearCharacter(WINDOW *win, Character character) {
    mvwprintw(win, character.y, character.x, " ");
    wrefresh(win);
}

// Função para mover o personagem na janela
void moveCharacter(WINDOW *win, Character *character, int dx, int dy) {

    // Salva as coordenadas atuais antes de mover
    int oldX = character->x;
    int oldY = character->y;

    // Calcula as novas coordenadas
    int newX = oldX + dx;
    int newY = oldY + dy;

    // Verifica se as novas coordenadas estão dentro dos limites da janela/mapa
    if (newX >= 0 && newX < 81 && newY >= 0 && newY < 18) {
        // Limpa a posição anterior do personagem
        mvwaddch(win, oldY, oldX, ' ');

        // Atualiza as coordenadas do personagem
        character->x = newX;
        character->y = newY;

        // Desenha o personagem na nova posição
        mvwaddch(win, newY, newX, character->symbol);

        // Atualiza a janela
        wrefresh(win);
    }
}

int isLinhaEmBranco(const char *linha) {
    while (*linha) {
        if (!isspace(*linha)) {
            return 0;  // A linha não está em branco
        }
        linha++;
    }
    return 1;  // A linha está em branco
}

void lerMapa(Game  *game) {
    char filepath[100];  // Ajuste o tamanho conforme necessário

    if (game->currentLevel== 1){
        snprintf(filepath, sizeof(filepath), "./maps/%s", "labN1.txt");
        //snprintf(filepath, sizeof(filepath), "../src/jogoUI/%s", "labirinto.txt");
    }
    if (game->currentLevel == 2){
        snprintf(filepath, sizeof(filepath), "./maps/%s", "labN2.txt");
    }
    if (game->currentLevel == 3){
        snprintf(filepath, sizeof(filepath), "./maps/%s", "labN3.txt");
    }

    FILE *filePointer;
    filePointer = fopen(filepath, "r");

    if (filePointer == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
    }

    int linhaAtual = 0;
    char buffer[MAP_COLS + 2];  // +2 para incluir espaço para '\n' e '\0'

    while (linhaAtual < MAP_LINES && fgets(buffer, sizeof(buffer), filePointer) != NULL) {
        size_t comprimento = strlen(buffer);

        // Remover o caractere de nova linha, se existir
        if (comprimento > 0 && buffer[comprimento - 1] == '\n') {
            buffer[comprimento - 1] = '\0';
        }

        // Verificar se a linha não está em branco
        if (!isLinhaEmBranco(buffer)) {
            strcpy(map.cmap[linhaAtual], buffer);
            linhaAtual++;
        }
    }

    fclose(filePointer);
}




void mostraMapa(WINDOW *mapawin, int height, int width, Character* player) {
    echo();
    int yMax, xMax,a;
    getmaxyx(stdscr, yMax, xMax);
    int startY = (0);
    int startX = (1);
    int limiteX = width;
    int limiteY = height;
    mapawin = newwin(limiteY, limiteX, startY, startX);
    box(mapawin,0,0);
    start_color();

    init_color(COLOR_CYAN,0-999,0-999,0-999);

    wattron(mapawin,COLOR_PAIR(1));

    //mvwprintw(*win, 1,1,"MAPA");

    ///////////////////////////////////////


    for (int i = 0; i < MAP_LINES; i++) {
        if (map.cmap[i][0] != '\0') {
            // Mostra a linha com espaçamento duplo na largura
            for (int j = 0; j < strlen(map.cmap[i]); j++) {
                mvwprintw(mapawin, i + 1, j * SPACING_FACTOR + 1, "%c", map.cmap[i][j]);
            }
        }
    }

    ////////////////////////////////////////
    wattroff(mapawin,COLOR_PAIR(1));


    wattron(mapawin,COLOR_WHITE);
    drawCharacter(mapawin,*player);
    wattroff(mapawin,COLOR_WHITE);

    noecho();
    refresh();
    wrefresh(mapawin);
}


void pedras(WINDOW *win, int height, int width,Game *game) {
    

    int yMax, xMax,a;
    getmaxyx(stdscr, yMax, xMax);

    int startY = (18);
    int startX = (28);
    int limiteX = (13);

    win = newwin(3, limiteX, startY, startX);
    box(win,0,0);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_color(COLOR_CYAN,0-999,0-999,0-999);

    wattron(win,COLOR_PAIR(1));
    mvwprintw(win, 1,1,"Pedras:");
    wattroff(win,COLOR_PAIR(1));
    
    wattron(win,COLOR_PAIR(2));
    mvwprintw(win, 1,9,"%d",game->nRocks);
    wattroff(win,COLOR_PAIR(2));

    refresh();
    wrefresh(win);
}

void bloqueios(WINDOW *win, int height, int width,Game *game) {
      

    int yMax, xMax,a;
    getmaxyx(stdscr, yMax, xMax);

    int startY = (18);
    int startX = (12);
    int limiteX = (15);

    win = newwin(3, limiteX, startY, startX);
    box(win,0,0);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_color(COLOR_CYAN,0-999,0-999,0-999);

    wattron(win,COLOR_PAIR(1));
    mvwprintw(win, 1,1,"Bloqueios:");
    wattroff(win,COLOR_PAIR(1));
    
    wattron(win,COLOR_PAIR(2));
    mvwprintw(win, 1,12,"%d",game->nBlocks);
    wattroff(win,COLOR_PAIR(2));

    refresh();
    wrefresh(win);
}

void nivel(WINDOW *win, int height, int width,Game *game) {

    int yMax, xMax,xMin, yMin,a;
    getmaxyx(stdscr, yMax, xMax);

    int startY = (18);
    int startX = (1);
    int limiteX = (10);

    win = newwin(3, limiteX, startY, startX);
    box(win,0,0);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_color(COLOR_CYAN,0-999,0-999,0-999);

    wattron(win,COLOR_PAIR(1));
    mvwprintw(win, 1,1,"Nivel:");
    wattroff(win,COLOR_PAIR(1));
    
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    wattron(win,COLOR_PAIR(2));
    mvwprintw(win, 1,8,"1");
    wattroff(win,COLOR_PAIR(2));

    refresh();
    wrefresh(win);
}


void jogadores(WINDOW *win, int height, int width,Game *game) {
     

    int yMax, xMax,a;
    getmaxyx(stdscr, yMax, xMax);

    int startY = (21);
    int startX = (1);
    int limiteX = (40);

    win = newwin(4, limiteX, startY, startX);
    box(win,0,0);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    init_color(COLOR_CYAN,0-999,0-999,0-999);

    wattron(win,COLOR_PAIR(1));
    mvwprintw(win, 1,1,"Jogadores:");
    wattroff(win,COLOR_PAIR(1));
    
    wattron(win,COLOR_PAIR(2));
    for (int i = 0; i < game->nPlayers; i++) {
        mvwprintw(win, 1,12,"%s", game->players[i].username);
    }
    wattroff(win,COLOR_PAIR(2));
    
    refresh();
    wrefresh(win);
}


void executeCommand(char *command,WINDOW *win, int height, int width,Game *game) {
    
    echo();
    

    int yMax, xMax,a;
    getmaxyx(stdscr, yMax, xMax);

    int startY = (18);
    int startX = (42);
    int limiteX = (40);

    win = newwin(7, limiteX, startY, startX);
    box(win,0,0);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_color(COLOR_CYAN,0-999,0-999,0-999);

    wattron(win,COLOR_PAIR(1));
    //mvwprintw(*win, 1,1,"Notificacao de Comando!");
    mvwprintw(win, 1,1,"%s",command);
    wattroff(win,COLOR_PAIR(1));

    noecho();
    refresh();
    wrefresh(win);
}


char* comandos(WINDOW * comandwin){
    curs_set(1);
    char comando[MAX_COMMAND_LENGTH];
    
    echo();
    

    int yMax, xMax,a;
    getmaxyx(stdscr, yMax, xMax);

    comandwin = newwin(3,81, 25,1);
    box(comandwin,0,0);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_color(COLOR_CYAN,0-999,0-999,0-999);

    wattron(comandwin,COLOR_PAIR(1));
    mvwprintw(comandwin, 1,1,"Comandos: ");
    wattroff(comandwin,COLOR_PAIR(1));

    wgetstr(comandwin,comando);

    noecho();
    refresh();
    wrefresh(comandwin);
    return strdup(comando);
}

void comandos2(WINDOW * comandwin){
    curs_set(0);
	raw();
    initscr();
    noecho();
    
    int yMax, xMax,a;
    getmaxyx(stdscr, yMax, xMax);

    comandwin = newwin(3,81, 25,1);
    box(comandwin,0,0);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_color(COLOR_CYAN,0-999,0-999,0-999);

    wattron(comandwin,COLOR_PAIR(1));
    mvwprintw(comandwin, 1,1,"Modo Jogo");
    wattroff(comandwin,COLOR_PAIR(1));

    refresh();
    wrefresh(comandwin);

   
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
WINDOW* processCommand(Game *game, char *command,WINDOW * comando) {

   	WINDOW *centeredWin;

    if (strcmp(command, "players") == 0) {
        /*
        printf("Lista de Jogadores:\n");
        for (int i = 0; i < game->nPlayers; i++) {
            printf("- %s\n", game->players[i].name);
        }*/
        executeCommand(command,centeredWin, 0, 0, game);
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
            executeCommand(cmdCopy,centeredWin, 0, 0,game);
        }
        else
        {
            executeCommand("msg <nome_utilizador> <mensagem>",centeredWin, 0, 0, game);
        }


    }
    //codes
    ////////////////////////////////////////////////
    else
    if (strcmp(command, "n1") == 0) {
        game->currentLevel=1;
        lerMapa(game);
    }
    else
    if (strcmp(command, "n2") == 0) {
        game->currentLevel=2;
        lerMapa(game);
    }
    else
    if (strcmp(command, "n3") == 0) {
        game->currentLevel=3;
        lerMapa(game);
    }
    ////////////////////////////////////////////////////77
    else
    if (strcmp(command, "exit") == 0) {
        // TODO: avisar motor e sair ordeiramente
        endwin();
        
        exit(0);
    }

    else
    {
        executeCommand("Comando invalido!",centeredWin, 0, 0, game);
    }
    
    return 0;
    
}

void destroyCharacter(Character *character) {
    free(character);
}

//Função para verificar se há colisão na próxima posição
bool verificaColisao(char mapa[][MAP_COLS + 1], int nextY, int nextX) {
    // Ajusta as coordenadas do jogador para corresponder ao mapeamento no mapa
    int playerMapY = nextY - 1;
    int playerMapX = nextX / 2;

    // Verifica se a próxima posição contém 'X' (obstáculo)
    return mapa[playerMapY][playerMapX] == 'X';
}

// Função para controlo de teclas
void controloTeclas(Game *game) {
    keypad(stdscr, TRUE);
    WINDOW * mapawin;
    WINDOW * nivelwin;
    WINDOW * jogadoreswin;
    WINDOW * bloqueioswin;
    WINDOW * pedraswin;
    WINDOW * Commandwin;
    WINDOW * notificationwin;
    
    Character player = initCharacter(3, 2, 'P'); 
    lerMapa(game);
    
    mostraMapa(mapawin, 18, 81, &player);
    nivel(nivelwin, 0, 0,game);
    jogadores(jogadoreswin, 0, 0, game);
    bloqueios(bloqueioswin, 0, 0,game);
    pedras(pedraswin, 0, 0,game);
    executeCommand(" ",notificationwin, 0, 0,game);
    char *command;

    comandos2(Commandwin);  
   
    int ch;
    
    int exitRequested = 0;
    while ((ch = getch()) != 'q') {
        int direction = 0;

        switch (ch) {
            case KEY_UP:
                
                if (!verificaColisao(map.cmap, player.y - 1, player.x) &&
                ((player.y-1) > 1) && 
                ((player.x) > 1) && 
                ((player.y-1) < 16) && 
                ((player.x) < 79)) 
                {          	   
                    (player.y)--;
                    mostraMapa(mapawin, 18, 81, &player);
                }
                break;
	    case KEY_DOWN:
		if (!verificaColisao(map.cmap, player.y + 1, player.x) &&
		((player.y+1) > 1) && 
                ((player.x) > 1) && 
                ((player.y+1) < 16) && 
                ((player.x) < 79)) 
                {
		    (player.y)++;
		    mostraMapa(mapawin, 18, 81, &player);
		}
		break;
	    case KEY_LEFT:
		if (!verificaColisao(map.cmap, player.y, player.x - 2) &&
		((player.y) > 1)&& 
                ((player.x-2) > 1) && 
                ((player.y) < 16) && 
                ((player.x-2) < 79))
		{
		    (player.x)--;
		    (player.x)--;
		    mostraMapa(mapawin, 18, 81, &player);
		}
		break;
	    case KEY_RIGHT:
		if (!verificaColisao(map.cmap, player.y, player.x + 1) &&
		((player.y) > 1)&& 
                ((player.x+2) > 1) && 
                ((player.y) < 16) && 
                ((player.x+2) < 79))
                {
		    (player.x)++;
		    (player.x)++;
		    mostraMapa(mapawin, 18, 81, &player);
		}
		break;          
            case ' ':                          
                    // Se estiver no modo de comando, execute o comando
                    command = comandos(Commandwin);
                    processCommand(game, command, Commandwin);
                    free( command);
                    comandos2(Commandwin);  
                                                                     
                 
                break;
            default:
                direction = 0;
                break;
        }
           
            
    }  
}

int main(int argc, char *argv[]) {
    Player activePlayers[MAX_ACTIVE_PLAYERS];

    Game game;
    game.currentLevel=1;
    game.nBlocks=0;
    game.nRocks=0;

    printf("\e[8;28;83t");

    initscr();
    noecho();
    keypad(stdscr, TRUE);
    
    if (argc < 2) {
        printf("Nome do jogador como argumento.\n");
        return 1;
    }

    strncpy(game.players[0].username, argv[1], MAX_PLAYER_NAME - 1);
    game.players[0].username[MAX_PLAYER_NAME - 1] = '\0';


    game.nPlayers++;

    
    runMenuLogic();
    	
    flushinp();
    
    
    controloTeclas(&game);
    
    //getch();
    endwin();

    return 0;
}

