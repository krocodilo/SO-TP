#include <pthread.h>

#include "windows_ncurses.h"
#include "jogoUI_backend.h"

Map currentMap;


// Função para controlo de teclas
void controloTeclas(Windows* windows, int generalPipe, int pid) {

    windows-> mapawin = newwin(18,81,0,1);
    windows-> nivelwin = newwin(3,15,18,1);
    windows-> bloqueioswin = newwin(3,15,24,1);
    windows-> pedraswin = newwin(3,15,21,1);
    windows-> Commandwin = newwin(3,81, 27,1);
    windows-> notificationwin = newwin(9,65,18,17);


    mostraMapa(windows->mapawin, currentMap);
    nivel(windows->nivelwin, 0);
    bloqueios(windows->bloqueioswin, 0);
    pedras(windows->pedraswin, 0);
    executeCommand(" ",windows->notificationwin);
    comandos2(windows->Commandwin);

    char *command;

    int ch;
    int exitRequested = 0;
//    while ((ch = getch()) != 'q') {
    while(true){
        ch = getch();
        switch (ch) {
            case KEY_UP:
            case KEY_DOWN:
            case KEY_LEFT:
            case KEY_RIGHT: {
                MoveRequestMessage msg = {pid, ch};
                writeMessage(generalPipe, MoveRequest, &msg, sizeof(msg));
                break;
            }
            case ' ': {
                // Se estiver no modo de comando, executa o comando
                command = comandos(windows->Commandwin);
                processCommand(command, windows->notificationwin, generalPipe);
                free(command);
                comandos2(windows->Commandwin);
                break;
            }
        }
    }
    terminate(EXIT_SUCCESS);
}


int main(int argc, char *argv[]) {
    printf("\e[8;30;83t");
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

    SignUpMessage userInfo;
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
    bool motorOrderedTerminate = false;
    saveInfo(&userInfo, generalPipe, &motorOrderedTerminate);

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

    // Start listening to messages from motor
    CommunicationsThreadArg arg = {
            .myPipe = myPipe,
            .map = &currentMap,
            .windows = &windows,
            .motorOrderedTerminate = &motorOrderedTerminate
    };
    pthread_t id;
    if( pthread_create(&id, NULL, communicationsThread, &arg) != 0 ){
        perror("\nERRO ao criar thread de comunicacoes.\n");
        terminate(EXIT_FAILURE);
    }

    ////////////////////////////////////////////

    initscr();
    noecho();
    keypad(stdscr, TRUE);
    flushinp();     // limpar input

    controloTeclas(&windows, generalPipe, userInfo.pid);

    endwin();

    close(myPipe);
    close(generalPipe);
    terminate(EXIT_SUCCESS);
    return 0;
}

