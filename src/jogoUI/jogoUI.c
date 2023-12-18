#include "windows_ncurses.h"

SignUpMessage signUp;

void terminate(int signum){
    unlink(signUp.pipePath);
}

int getNextMessageType(int pipe_fd){
    int msgType = -1;

    if (read(pipe_fd, &msgType, sizeof(int)) != sizeof(int) ){
        perror("\nERRO: foi recebida uma mensagem incompleta.\n");
        return -1;
    }
    return msgType;
}

bool getMessage(int pipe_fd, void* buffer, int size){
    if (read(pipe_fd, buffer, size) != sizeof(int) ){
        perror("\nERRO: foi recebida uma mensagem incompleta.\n");
        return false;
    }
    return true;
}

void run(int myPipe){

    int msgType = getNextMessageType(myPipe);
    if(msgType == -1)
        return;

    Map map;

    if( getMessage(myPipe, &map, sizeof(Map)) == false)
        return;
}

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
    
    Character player = initCharacter(3, 15, 'H');

    //lerMapa


    
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

    printf("\e[8;28;83t");

    initscr();
    noecho();
    keypad(stdscr, TRUE);
    
    ////////////////////////////////////////////

    // Register signal handler
    signal(SIGINT, terminate);
    signal(SIGTERM, terminate);


    if (argc < 2) {
        perror("Nome do jogador como argumento.\n");
        return 1;
    }
    if(strlen(argv[1]) > MAX_PLAYER_NAME){
        fprintf(stderr, "Nome do jogador e demasiado longo. Maximo de %d caracteres.", MAX_PLAYER_NAME);
        return 1;
    }

    strncpy(signUp.username, argv[1], MAX_PLAYER_NAME);
    strcpy(signUp.pipePath, PIPE_DIRECTORY);
    strcat(signUp.pipePath, signUp.username);

    // Create and open general pipe
    int myPipe = create_and_open(signUp.pipePath, O_RDWR);
    if (myPipe == -1){
        perror("\nERRO: nao foi possivel abrir o pipe deste cliente.\n");
        return -1;
    } else if (myPipe == -2) {
        printf("\nJa existe um utilizador com este username.\n");
        return -1;
    } else if (myPipe == -3) {
        perror("\nERRO: falha ao criar o pipe deste cliente.\n");
        return -1;
    }

    int generalPipe = open(GENERAL_PIPE, O_WRONLY);
    if (generalPipe == -1){
        perror("\nERRO ao tentar abrir o pipe geral.\n");
        return -1;
    }

    write(generalPipe, &signUp, sizeof(signUp));


    printf("waiting");

    sleep(1000000);

    close(myPipe);
    close(generalPipe);

    ////////////////////////////////////////////

    runMenuLogic();
    	
    flushinp();
    
    
    controloTeclas();
    
    //getch();
    endwin();

    return 0;
}

