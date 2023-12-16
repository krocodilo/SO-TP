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

void terminate(int signum){
    unlink(signUp.pipePath);
}

int main(int argc, char *argv[]) {


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
    return 0;
}