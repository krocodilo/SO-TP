#include <pthread.h>

#include "windows_ncurses.h"
#include "jogoUI_backend.h"


SignUpMessage userInfo;
Map allMaps;


void terminate(int exitcode){
    printf("\nTerminating...\n");

    unlink(userInfo.pipePath);
    endwin();
    fflush(stdout);
    exit(exitcode);
}


// Função para controlo de teclas
void controloTeclas(Windows* windows, int generalPipe) {
//    keypad(stdscr, TRUE);

    //newwin(limiteY, limiteX, startY, startX);

    windows-> mapawin = newwin(18,81,0,1);
    windows-> nivelwin = newwin(3,15,18,1);
    windows-> bloqueioswin = newwin(3,15,24,1);
    windows-> pedraswin = newwin(3,15,21,1);
    windows-> Commandwin = newwin(3,81, 27,1);
    windows-> notificationwin = newwin(9,65,18,17);



    mostraMapa(windows->mapawin, 18, 81, NULL, allMaps);
    nivel(windows->nivelwin, 0);
    bloqueios(windows->bloqueioswin, 0);
    pedras(windows->pedraswin, 0);
    executeCommand(" ",windows->notificationwin, 0, 0);
    char *command;

    comandos2(windows->Commandwin);
   
    int ch;
    int exitRequested = 0;
    while ((ch = getch()) != 'q') {
        switch (ch) {
            case KEY_UP:
            case KEY_DOWN:
            case KEY_LEFT:
            case KEY_RIGHT: {
                MoveRequestMessage msg = {userInfo.pid, ch};
                writeMessage(generalPipe, MoveRequest, &msg, sizeof(msg));
                break;
            }
            case ' ': {
                // Se estiver no modo de comando, execute o comando
                command = comandos(windows->Commandwin);
                processCommand(command, windows->Commandwin, generalPipe);
                free(command);
                comandos2(windows->Commandwin);
                break;
            }
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

    strncpy(userInfo.username, argv[1], MAX_PLAYER_NAME-1);
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
////            memcpy(&allMaps, &msg.allMaps, sizeof(Map));
//            copyMap(&allMaps, &msg.allMaps);
//            break;
//        }
//        default:
//            perror("\nErro ao ler o tipo da proxima mensagem no pipe.");
//            terminate(1);
//    }

    // Start listening to messages from motor
    CommunicationsThreadArg arg = {
            .myPipe = myPipe,
            .map = &allMaps,
            .windows = &windows
    };
    pthread_t id;
    if( pthread_create(&id, NULL, communicationsThread, &arg) != 0 ){
        perror("\nERRO ao criar thread de comunicacoes.\n");
        terminate(EXIT_FAILURE);
    }

    ////////////////////////////////////////////

    printf("\e[8;30;83t");

    initscr();
    noecho();
    keypad(stdscr, TRUE);
    flushinp();     // limpar input

    //menu de jogo
    //runMenuLogic();

    controloTeclas(&windows, generalPipe);
    
    //getch();
    endwin();

    close(myPipe);
    close(generalPipe);
    terminate(EXIT_SUCCESS);
    return 0;
}

