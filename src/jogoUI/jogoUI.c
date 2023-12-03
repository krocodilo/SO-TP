#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "../motor/data_structs.h"

#define MAX_NAME_LENGTH 50
#define MAX_COMMAND_LENGTH 50
#define MAX_ACTIVE_PLAYERS 5
#define SPACING_FACTOR 2

//menu inicial
#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 3

Mapa mapa;

int showMenu(char **choices, int n_choices, int *highlight);
int runMenuLogic();
WINDOW* processCommand(Game *game, char *command,WINDOW *centeredWin) ;
//int validatePlayerName(Player activePlayers[], int numActivePlayers, char *name);
void listBots(Game *game);
void destroy_win(WINDOW *local_win);


typedef struct p Character, *pCharacter;

struct p{
    int x;
    int y;
    char symbol;
};

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
    //clearCharacter(win, *character);
    //character->x += dx;
    //character->y += dy;
    //drawCharacter(win, *character);
    
    
    
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




//Menu inicial
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




void destroy_win(WINDOW *local_win){	
  
      wborder (local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
  
    wrefresh(local_win);
    delwin(local_win);
}

void lerMapa() {
    char filepath[100];  // Ajuste o tamanho conforme necessário
    snprintf(filepath, sizeof(filepath), "../src/jogoUI/%s", "labirinto.txt");

    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo de mapa");
        return;
    }

    int i = 0;
    while (i < MAP_LINES && fgets(mapa.map[i], sizeof(mapa.map[i]), file) != NULL) {
        // Remove a nova linha do final de cada linha lida
        mapa.map[i][strcspn(mapa.map[i], "\n")] = '\0';

        // Ignora linhas em branco
        if (mapa.map[i][0] != '\0') {
            i++;
        }
    }

    fclose(file);
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
        if (mapa.map[i][0] != '\0') {
            // Mostra a linha com espaçamento duplo na largura
            for (int j = 0; j < strlen(mapa.map[i]); j++) {
                mvwprintw(mapawin, i + 1, j * SPACING_FACTOR + 1, "%c", mapa.map[i][j]);
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

void pedras(WINDOW *win, int height, int width) {
    

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
    mvwprintw(win, 1,9,"5");
    wattroff(win,COLOR_PAIR(2));

    refresh();
    wrefresh(win);
}

void bloqueios(WINDOW *win, int height, int width) {
      

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
    mvwprintw(win, 1,12,"3");
    wattroff(win,COLOR_PAIR(2));

    refresh();
    wrefresh(win);
}

void nivel(WINDOW *win, int height, int width) {
      

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


void jogadores(WINDOW *win, int height, int width) {
     

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
    mvwprintw(win, 1,12,"Duraes");
    /*
    for (int i = 0; i < game->nPlayers; i++) {
        mvwprintw(*win, 1,1,"%s", game->players[i].name);
    }*/
    wattroff(win,COLOR_PAIR(2));
    
    wattron(win,COLOR_PAIR(2));
    mvwprintw(win, 1,12,"Duraes");
    wattroff(win,COLOR_PAIR(2));
    
    refresh();
    wrefresh(win);
}


void executeCommand(char *command,WINDOW *win, int height, int width) {
    
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
        executeCommand(command,centeredWin, 0, 0);
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
            executeCommand(cmdCopy,centeredWin, 0, 0);
        }
        else
        {
            executeCommand("msg <nome_utilizador> <mensagem>",centeredWin, 0, 0);
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
        executeCommand("Comando invalido!",centeredWin, 0, 0);
    }
    
    return 0;
    
}

void destroyCharacter(Character *character) {
    free(character);
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
    lerMapa();
    
    mostraMapa(mapawin, 18, 81, &player);
    nivel(nivelwin, 0, 0);
    jogadores(jogadoreswin, 0, 0);
    bloqueios(bloqueioswin, 0, 0);
    pedras(pedraswin, 0, 0);
    executeCommand(" ",notificationwin, 0, 0);
    

    comandos2(Commandwin);  
   
    int ch;
    
    int exitRequested = 0;
    while ((ch = getch()) != 'q') {
        int direction = 0;

        switch (ch) {
            case KEY_UP:
                if (mapa.map[player.y - 1][player.x] != 'X') {
                (player.y)--;
                mostraMapa(mapawin, 18, 81, &player);
                }
                break;
	    case KEY_DOWN:
		if (mapa.map[player.y + 1][player.x] != 'X') {
		    (player.y)++;
		    mostraMapa(mapawin, 18, 81, &player);
		}
		break;
	    case KEY_LEFT:
		if (mapa.map[player.y][player.x - 2] != 'X') {
		    (player.x)--;
		    (player.x)--;
		    mostraMapa(mapawin, 18, 81, &player);
		}
		break;
	    case KEY_RIGHT:
		if (mapa.map[player.y][player.x + 2] != 'X') {
		    (player.x)++;
		    (player.x)++;
		    mostraMapa(mapawin, 18, 81, &player);
		}
		break;
            break;
            case ' ':                          
                    // Se estiver no modo de comando, execute o comando
                    char *command = comandos(Commandwin);
                    processCommand(game, command, Commandwin);
                    free(command);
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
    char *command;

    Game game;
    
    char playerName[MAX_NAME_LENGTH];

    initscr();
    noecho();
    keypad(stdscr, TRUE);
    
    if (argc < 2) {
        printf("Nome do jogador como argumento.\n");
        return 1;
    }

    strncpy(playerName, argv[1], MAX_NAME_LENGTH - 1);
    playerName[MAX_NAME_LENGTH - 1] = '\0';

    //mvprintw(5, 0, "Jogador %s criado!", playerName);
    
    
    runMenuLogic();
    	
    flushinp();
    
    
    controloTeclas(&game);
    
    //getch();
    endwin();

    return 0;
}

