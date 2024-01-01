#include "windows_ncurses.h"


void terminate(int exitcode) {
    endwin();
    printf("\nTerminating...\n");

    unlink(userInfo.pipePath);
    fflush(stdout);
    exit(exitcode);
}


//apaga window
void destroy_win(WINDOW *local_win){
    wborder (local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');

    wrefresh(local_win);
    delwin(local_win);
}

//mostra mapa
void mostraMapa(WINDOW *mapawin, int height, int width, Character* player, Map map) {
    echo();
    curs_set(0);
    box(mapawin,0,0);

    start_color();

    init_color(COLOR_CYAN,0-999,0-999,0-999);

    wattron(mapawin,COLOR_PAIR(1));

    for (int i = 0; i < MAP_LINES; i++) {
        if (map.cmap[i][0] != '\0') {
            // Mostra a linha com espaçamento duplo na largura
            for (int j = 0; j < strlen(map.cmap[i]); j++) {
                mvwprintw(mapawin, i + 1, j * SPACING_FACTOR + 1, "%c", map.cmap[i][j]);
            }
        }
    }

    wattroff(mapawin,COLOR_PAIR(1));

    noecho();
    refresh();
    wrefresh(mapawin);
}


//mostra numero de pedras no mapa numa janela
void pedras(WINDOW *win, int nRocks) {

    box(win,0,0);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_color(COLOR_CYAN,0-999,0-999,0-999);

    wattron(win,COLOR_PAIR(1));
    mvwprintw(win, 1,1,"Pedras:");
    wattroff(win,COLOR_PAIR(1));

    wattron(win,COLOR_PAIR(2));

    char string[2];
    sprintf(string, "%d", nRocks);
    mvwprintw(win, 1, 12, "%s", string);
    wattroff(win,COLOR_PAIR(2));

    refresh();
    wrefresh(win);
}

//mostra numero de bloqueios no mapa numa janela
void bloqueios(WINDOW *win, int nMBlocks) {

    box(win,0,0);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_color(COLOR_CYAN,0-999,0-999,0-999);

    wattron(win,COLOR_PAIR(1));
    mvwprintw(win, 1,1,"Bloqueios:");
    wattroff(win,COLOR_PAIR(1));

    wattron(win,COLOR_PAIR(2));

    char string[2];
    sprintf(string, "%d", nMBlocks);
    mvwprintw(win, 1, 12, "%s", string);
    wattroff(win,COLOR_PAIR(2));

    refresh();
    wrefresh(win);
}

//mostra nivel do jogo numa janela *precisa de ser visto
void nivel(WINDOW *win, int level) {

    box(win,0,0);

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_color(COLOR_CYAN,0-999,0-999,0-999);

    wattron(win,COLOR_PAIR(1));
    mvwprintw(win, 1, 1, "Nivel:");
    wattroff(win,COLOR_PAIR(1));

    init_pair(2, COLOR_CYAN, COLOR_BLACK);
    wattron(win,COLOR_PAIR(2));

    char string[2];
    sprintf(string, "%d", level);
    mvwprintw(win, 1, 12, "%s", string);
    wattroff(win,COLOR_PAIR(2));

    refresh();
    wrefresh(win);
}

//mostra nome de jogadores no mapa numa janela *não é utilizado
//void jogadores(WINDOW *win, char *playersCommaSeparated) {
//
//    box(win,0,0);
//
//    start_color();
//    init_pair(1, COLOR_GREEN, COLOR_BLACK);
//    init_pair(2, COLOR_CYAN, COLOR_BLACK);
//    init_color(COLOR_CYAN,0-999,0-999,0-999);
//
//    wattron(win,COLOR_PAIR(1));
//    mvwprintw(win, 1,1,"Jogadores:");
//    wattroff(win,COLOR_PAIR(1));
//
//    wattron(win,COLOR_PAIR(2));
//    mvwprintw(win, 1, 12, "%s", playersCommaSeparated);
//    wattroff(win,COLOR_PAIR(2));
//
//    refresh();
//    wrefresh(win);
//}

//mostra O COMANDO ACIONADO
void executeCommand(char *command,WINDOW *win) {
    echo();
    werase(win);
    refresh();
    wrefresh(win);
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
    char comando[MAX_COMMAND_LENGTH + MAX_MESSAGE_SIZE];

    echo();

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
    werase(comandwin);
    curs_set(0);
    raw();
    initscr();
    noecho();

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
WINDOW* processCommand(char *command, WINDOW *window, int generalPipe, const char *senderUsername) {


    if (strncmp(command, "players", 7) == 0) {
        GenericRequestMessage msg = {getpid()};
        if( ! writeMessage(generalPipe, GetPlayersList, &msg, sizeof(msg)) )
            executeCommand("Erro ao enviar mensagem para pipe do motor.", window);
    }

    else if (strncmp(command, "msg", 3) == 0) {

        char *cmd = strtok(command, " ");
        char *targetName = strtok(NULL, " ");
        char *message = strtok(NULL, "");

        if (targetName != NULL && message != NULL) {

            char userPipeName[PIPE_PATH_MAX_SIZE];
            strcpy(userPipeName, PIPE_DIRECTORY);
            strcat(userPipeName, targetName);

            int userPipe = open(userPipeName, O_WRONLY);
            if (userPipe == -1){
                executeCommand("Nome de jogador não existente", window);
                return NULL;
            }

            TextMessage msg;
            strncpy(msg.from, senderUsername, MAX_PLAYER_NAME);
            strncpy(msg.message, message, MAX_MESSAGE_SIZE);

            if (!writeMessage(userPipe, TextMsg, &msg, sizeof(msg))) {
                executeCommand("ERRO ao enviar a mensagem para o destinatário.", window);
                close(userPipe);
                return NULL;
            }
            close(userPipe);

            /*
           // Validar se o nome do destinatário existe
            if (validatePlayerName(game->players, game->nPlayers, targetName)) {
                mvprintw(12, 0, "Mensagem enviada para %s: %s", targetName, message);
            } else {
                mvprintw(12, 0, "Erro: Utilizador %s não encontrado.", targetName);
            }*/
        }
        else
            executeCommand("SINTAXE: msg <nome_utilizador> <mensagem>", window);
    }

    else if (strcmp(command, "exit") == 0) {
        terminate(EXIT_SUCCESS);
    }

    else
    {
        executeCommand("Comando invalido!", window);
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