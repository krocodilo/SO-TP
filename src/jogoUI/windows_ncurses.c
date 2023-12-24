#include "windows_ncurses.h"


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

//lógica do menu
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


//apaga window
void destroy_win(WINDOW *local_win){

    wborder (local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');

    wrefresh(local_win);
    delwin(local_win);
}

//mostra mapa
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


//    for (int i = 0; i < MAP_LINES; i++) {
//        if (map.cmap[i][0] != '\0') {
//            // Mostra a linha com espaçamento duplo na largura
//            for (int j = 0; j < strlen(map.cmap[i]); j++) {
//                mvwprintw(mapawin, i + 1, j * SPACING_FACTOR + 1, "%c", map.cmap[i][j]);
//            }
//        }
//    }









    ////////////////////////////////////////






    wattroff(mapawin,COLOR_PAIR(1));


    wattron(mapawin,COLOR_WHITE);
    drawCharacter(mapawin,*player);
    wattroff(mapawin,COLOR_WHITE);

    noecho();
    refresh();
    wrefresh(mapawin);
}

//mostra numero de pedras no mapa numa janela
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
    mvwprintw(win, 1,9," ");
    wattroff(win,COLOR_PAIR(2));

    refresh();
    wrefresh(win);
}

//mostra numero de bloqueios no mapa numa janela
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
    mvwprintw(win, 1,12," ");
    wattroff(win,COLOR_PAIR(2));

    refresh();
    wrefresh(win);
}

//mostra nivel do jogo numa janela *precisa de ser visto
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

//mostra nome de jogadores no mapa numa janela *precisa de ser visto
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
   // for (int i = 0; i < game->nPlayers; i++) {
     //   mvwprintw(win, 1,12,"%s", game->players[i].username);
    //}
    wattroff(win,COLOR_PAIR(2));

    refresh();
    wrefresh(win);
}

//mostra O COMANDO ACIONADO
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

//modo escrita de comando
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

//modo jogo
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

// Função para processar comandos do jogador
WINDOW* processCommand(char *command,WINDOW * comando) {

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
        //codes
        ////////////////////////////////////////////////
    else
    if (strcmp(command, "n1") == 0) {
        //game->currentLevel=1;
        //lerMapa(game);
    }
    else
    if (strcmp(command, "n2") == 0) {
        //game->currentLevel=2;
        //lerMapa(game);
    }
    else
    if (strcmp(command, "n3") == 0) {
        //game->currentLevel=3;
        //lerMapa(game);
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
        executeCommand("Comando invalido!",centeredWin, 0, 0);
    }

    return 0;

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

//elimina Caracter
void destroyCharacter(Character *character) {
    free(character);
}