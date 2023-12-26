#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#include "../common/utils.h"
#include "../common/constants.h"
#include "../common/messages.h"

SignUpMessage signUp;

void terminate(int exitcode){
    unlink(signUp.pipePath);
    printf("\nTerminate");
    fflush(stdout);
    exit(exitcode);
}
//
//#include "../motor/motor_init.h"

int main(int argc, char *argv[]) {

//    Map maps[MAX_LEVELS];
//
//    readMapFiles(maps);
//
//
//    for (int i = 0; i < MAP_LINES; i++) {
//        printf("\n%s", maps[0].cmap[i]);
//    }

//    exit(0);


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

    strncpy(signUp.username, argv[1], MAX_PLAYER_NAME);
    strcpy(signUp.pipePath, PIPE_DIRECTORY);
    strcat(signUp.pipePath, signUp.username);

    // Create and open general pipe
    int myPipe = create_and_open(signUp.pipePath, O_RDWR);
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

    int generalPipe = open(GENERAL_PIPE, O_WRONLY);
    if (generalPipe == -1){
        perror("\nERRO ao tentar abrir o pipe geral.\n");
        terminate(EXIT_FAILURE);
    }

    if( ! writeMessage(generalPipe, SignUp, &signUp, sizeof(signUp)) ){
        perror("\nErro ao enviar mensagem de inscricao para o motor.");
        terminate(EXIT_FAILURE);
    }

    printf("\nFoi enviada mensagem de inscricao.\n");

    int type = readNextMessageType(myPipe);
    if( type != SignUpSuccessful ) {
        fprintf(stderr, "\nERRO, esperava receber mensagem de sucesso. Recebu mensagem #%d", type);
        terminate(EXIT_FAILURE);
    }

    switch (readNextMessageType(myPipe)) {
        case NewLevel: {
            NewLevelMessage msg;
            if( ! readNextMessage(myPipe, &msg, sizeof(msg)) ){
                perror("\nErro ao ler a proxima mensagem no pipe.");
                break;
            }
            for(int i = 0; i < MAP_LINES; i++){
                printf("%s\n", msg.map.cmap[i]);
            }
            break;
        }
        default:
            perror("\nErro ao ler o tipo da proxima mensagem no pipe.");
    }

//    sleep(1000000);

    close(myPipe);
    close(generalPipe);
    unlink(signUp.pipePath);
    return 0;
}