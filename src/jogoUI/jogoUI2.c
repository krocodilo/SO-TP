#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "../common/utils.h"
#include "../common/constants.h"


int main(int argc, char *argv[]) {

    char username[MAX_PLAYER_NAME];

    if (argc < 2) {
        perror("Nome do jogador como argumento.\n");
        return 1;
    }
    if(strlen(argv[1]) > MAX_PLAYER_NAME){
        fprintf(stderr, "Nome do jogador e demasiado longo. Maximo de %d caracteres.", MAX_PLAYER_NAME);
        return 1;
    }

    char myPipeName[MAX_PLAYER_NAME + strlen(PIPE_DIRECTORY)];
    strncpy(username, argv[1], MAX_PLAYER_NAME);
    strcpy(myPipeName, PIPE_DIRECTORY);
    strcat(myPipeName, username);

    // Create and open general pipe
    int myPipe = create_and_open(myPipeName, O_RDWR);
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

    int pipegen_fd = open(GENERAL_PIPE, O_WRONLY);
    if (pipegen_fd == -1){
        perror("\nERRO ao tentar abrir o pipe geral.\n");
        return -1;
    }

    write(pipegen_fd, &username, sizeof(username));
    close(pipegen_fd);

    printf("waiting");

    sleep(1000000);

    close(myPipe);
    unlink(myPipeName);
    return 0;
}