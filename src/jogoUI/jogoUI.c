#include <pthread.h>

#include "windows_ncurses.h"
#include "jogoUI_backend.h"


SignUpMessage userInfo;
Map map;


void terminate(int exitcode){
    printf("\nTerminating...\n");

    unlink(userInfo.pipePath);
    endwin();
    fflush(stdout);
    exit(exitcode);
}


//Função para verificar se há colisão na próxima posição
bool verificaColisao(char mapa[][MAP_COLS + 1], int nextY, int nextX) {
    // Ajusta as coordenadas do jogador para corresponder ao mapeamento no mapa
    int playerMapY = nextY - 1;
    int playerMapX = nextX / 2;

    // Verifica se a próxima posição contém 'X' (obstáculo)
    return mapa[playerMapY][playerMapX] == 'X';
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
void controloTeclas(Windows* windows) {
//    keypad(stdscr, TRUE);
    
    Character player = initCharacter(3, 15, 'H');


    
    mostraMapa(windows->mapawin, 18, 81, &player, map);
    nivel(windows->nivelwin, 0, 0);
    jogadores(windows->jogadoreswin, 0, 0);
    bloqueios(windows->bloqueioswin, 0, 0);
    pedras(windows->pedraswin, 0, 0);
    executeCommand(" ",windows->notificationwin, 0, 0);
    char *command;

    comandos2(windows->Commandwin);
   
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
                    mostraMapa(windows->mapawin, 18, 81, &player, map);
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
		    mostraMapa(windows->mapawin, 18, 81, &player, map);
		}
		break;
	    case KEY_LEFT:
		if (!verificaColisao(map.cmap, player.y, player.x - 2) &&
            ((player.y) > 1) &&
            ((player.x-2) > 1) &&
            ((player.y) < 16) &&
            ((player.x-2) < 79))
		{
		    (player.x)--;
		    (player.x)--;
		    mostraMapa(windows->mapawin, 18, 81, &player, map);
		}
		break;
	    case KEY_RIGHT:
		if (!verificaColisao(map.cmap, player.y, player.x + 1) &&
            ((player.y) > 1) &&
            ((player.x+2) > 1) &&
            ((player.y) < 16) &&
            ((player.x+2) < 79))
                {
		    (player.x)++;
		    (player.x)++;
		    mostraMapa(windows->mapawin, 18, 81, &player, map);
		}
		break;          
            case ' ':                          
                    // Se estiver no modo de comando, execute o comando
                    command = comandos(windows->Commandwin);
                    processCommand( command, windows->Commandwin);
                    free( command);
                    comandos2(windows->Commandwin);
                                                                     
                 
                break;
            default:
                direction = 0;
                break;
        }
    }  
}


int main(int argc, char *argv[]) {
    Windows windows;

    // Register signal handler
    signal(SIGINT, terminate);
    signal(SIGTERM, terminate);


    if (argc < 2) {
        perror("Nome do jogador como argumento.\n");
        return EXIT_FAILURE;
    }
    if(strlen(argv[1]) > MAX_PLAYER_NAME){
        fprintf(stderr, "Nome do jogador e demasiado longo. Maximo de %d caracteres.", MAX_PLAYER_NAME);
        return EXIT_FAILURE;
    }

    strncpy(userInfo.username, argv[1], MAX_PLAYER_NAME);
    strcpy(userInfo.pipePath, PIPE_DIRECTORY);
    strcat(userInfo.pipePath, userInfo.username);

    // Create and open my pipe
    int myPipe = create_and_open(userInfo.pipePath, O_RDWR);
    if (myPipe == -1){
        perror("\nERRO: nao foi possivel abrir o pipe deste cliente.\n");
        terminate(EXIT_FAILURE);
    } else if (myPipe == -2) {
        printf("\nJa existe um utilizador com este username.\n");
        return EXIT_FAILURE;
    } else if (myPipe == -3) {
        perror("\nERRO: falha ao criar o pipe deste cliente.\n");
        return EXIT_FAILURE;
    }

    // Open Motor's pipe
    int generalPipe = open(GENERAL_PIPE, O_WRONLY);
    if (generalPipe == -1){
        perror("\nERRO ao tentar abrir o pipe geral.\n");
        terminate(EXIT_FAILURE);
    }

    userInfo.pid = getpid();

    // Enviar inscrição para o motor
    if( ! writeMessage(generalPipe, SignUp, &userInfo, sizeof(userInfo)) ){
        perror("\nErro ao enviar mensagem de inscricao para o motor.");
        terminate(EXIT_FAILURE);
    }
    printf("\nFoi enviada mensagem de inscricao.");
    fflush(stdout);

    // Wait for confirmation
    int type = readNextMessageType(myPipe);
    if( type != SignUpSuccessful ) {
        fprintf(stderr, "\nERRO, esperava receber mensagem de sucesso. Recebu mensagem #%d", type);
        terminate(EXIT_FAILURE);
    }

//    switch (readNextMessageType(myPipe)) {
//        case NewLevel: {
//            NewLevelMessage msg;
//            if( ! readNextMessage(myPipe, &msg, sizeof(msg)) ){
//                perror("\nErro ao ler a proxima mensagem no pipe.");
//                break;
//            }
////            memcpy(&map, &msg.map, sizeof(Map));
//            copyMap(&map, &msg.map);
//            break;
//        }
//        default:
//            perror("\nErro ao ler o tipo da proxima mensagem no pipe.");
//            terminate(1);
//    }

    // Start listening to messages from motor
    CommunicationsThreadArg arg = {
            .myPipe = myPipe,
            .map = &map,
            .windows = &windows
    };
    pthread_t id;
    if( pthread_create(&id, NULL, communicationsThread, &arg) != 0 ){
        perror("\nERRO ao criar thread de comunicacoes.\n");
        terminate(EXIT_FAILURE);
    }

    ////////////////////////////////////////////

    printf("\e[8;28;83t");

    initscr();
    noecho();
    keypad(stdscr, TRUE);
    flushinp();     // limpar input

    //menu de jogo
    //runMenuLogic();


    controloTeclas(&windows);
    
    //getch();
    endwin();

    close(myPipe);
    close(generalPipe);
    terminate(EXIT_SUCCESS);
    return 0;
}

