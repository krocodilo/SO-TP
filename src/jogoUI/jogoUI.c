#include "windows_ncurses.h"

// Função para inicializar o personagem
Character initCharacter(int x, int y, char symbol) {
    Character character;
    character.x = x;
    character.y = y;
    character.symbol = symbol;
    return character;
}

// Função para controlo de teclas
void controloTeclas() {
    keypad(stdscr, TRUE);
    WINDOW * mapawin;
    WINDOW * nivelwin;
    WINDOW * jogadoreswin;
    WINDOW * bloqueioswin;
    WINDOW * pedraswin;
    WINDOW * Commandwin;
    WINDOW * notificationwin;
    
    Character player = initCharacter(3, 2, 'P'); 
    //lerMapa(game);
    
    mostraMapa(mapawin, 18, 81, &player);
    nivel(nivelwin, 0, 0);
    jogadores(jogadoreswin, 0, 0);
    bloqueios(bloqueioswin, 0, 0);
    pedras(pedraswin, 0, 0);
    executeCommand(" ",notificationwin, 0, 0);
    char *command;

    comandos2(Commandwin);  
   
    int ch;
    
    int exitRequested = 0;
    while ((ch = getch()) != 'q') {
        int direction = 0;

        switch (ch) {
            case KEY_UP:
                
                if (//!verificaColisao(map.cmap, player.y - 1, player.x) &&
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
		if (//!verificaColisao(map.cmap, player.y + 1, player.x) &&
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
		if (//!verificaColisao(map.cmap, player.y, player.x - 2) &&
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
		if (//!verificaColisao(map.cmap, player.y, player.x + 1) &&
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
                    processCommand( command, Commandwin);
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
    
    
    controloTeclas();
    
    //getch();
    endwin();

    return 0;
}

