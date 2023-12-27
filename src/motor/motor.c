#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>

#include "../common/constants.h"
#include "data_structs.h"
#include "motor_init.h"
#include "motor_signup.h"
#include "motor_game.h"
#include "motor_bots.h"


Game* game;
Map map[MAX_LEVELS];


void terminate(int exitcode){
    printf("\nTerminating...\n");

    // wait for all threads to join

    // Terminate all bots
    for( ;game->nBots > 0; game->nBots-- ){
        kill(game->bots[game->nBots-1].pid, SIGINT);
//        wait(???); // esperar que os bots terminem
    }

    // Close general pipe
    if(game->generalPipe > 0)
        close(game->generalPipe);
    unlink(GENERAL_PIPE);

    // Close client pipes

    free(game);
    fflush(stdout);
    exit(exitcode);
}



int processAdminCommand(char *adminCommand, GameSettings *gameSettings) {

//    if (strcmp(adminCommand, "run_bots") == 0) {
//        runBots(game);
//    } else

    if (strcmp(adminCommand, "users") == 0) {
        printf("\nComando \"users\"\n");
    }

    else if (strncmp(adminCommand, "kick", 4) == 0) {

        char *token = strtok(adminCommand, " ");
        char *targetName = strtok(NULL, " ");

        if (targetName != NULL) {

            char *aux;
            aux = (char *)malloc(strlen(targetName) + strlen(" banido") + 1);
            strcpy(aux,targetName);
            strcat(aux," banido");
            printf("\n");
            puts(aux);
            printf("\n");
            free(aux);
        }
        else {
            printf("\nkick <nome do jogador>\n");
        }
    }

    else if (strcmp(adminCommand, "bots") == 0) {
        printf("\nComando \"bots\"\n");
    }

    else if (strcmp(adminCommand, "bmov") == 0) {
        printf("\nComando \"bmov\"\n");
    }

    else if (strcmp(adminCommand, "rbm") == 0) {
        printf("\nComando \"rbm\"\n");
    }

    else if (strcmp(adminCommand, "begin") == 0) {
        printf("\nComando \"begin\"\n");
    }

    else if (strcmp(adminCommand, "end") == 0) {
        return -1;
    }

    else {
        printf("Comando do administrador inválido: %s\n", adminCommand);
    }
    return 0;
}






int main(int argc, char *argv[]) {
    GameSettings gameSettings;


    // Inicialize o gerador de números aleatórios com uma semente
    srand((unsigned int)time(NULL));

    // Read all map
    if(readMapFiles(map) == EXIT_FAILURE )
        return EXIT_FAILURE;

    // Initialize Game Structure
    game = malloc(sizeof(Game));
    if (game == NULL){
        perror("\nERRO: ao alocar memoria para o jogo.\n");
        return EXIT_FAILURE;
    }
    resetGame(game);

    // Read Environment Variables
    if( readEnvironmentVariables(&gameSettings) == EXIT_FAILURE )
        terminate(EXIT_FAILURE);

    // Register signal handler
    signal(SIGINT, terminate);
    signal(SIGTERM, terminate);

    // Create and open general pipe
    game->generalPipe = create_and_open(GENERAL_PIPE, O_RDWR);

    if (game->generalPipe == -1){
        perror("\nERRO: nao foi possivel abrir o mecanismo de comunicacao (pipe) geral.\n");
        terminate(EXIT_FAILURE);
    } else if (game->generalPipe == -2) {
        printf("\nJa existe uma instancia do Motor a executar! So e permitido uma!\n");
        return EXIT_FAILURE;
    } else if (game->generalPipe == -3) {
        perror("\nERRO: falha ao criar o mecanismo de comunicacao (pipe) geral.\n");
        return EXIT_FAILURE;
    }

    // create mutexes
    Mutexes mutexes;
    pthread_mutex_init(&mutexes.currentMap, NULL);
    pthread_mutex_init(&mutexes.players, NULL);

    while(true){
        if( waitForClientsSignUp(gameSettings, game) == EXIT_FAILURE) {
            terminate(EXIT_FAILURE);
        }

        // Make sure mutexes are unlocked
        pthread_mutex_unlock(&mutexes.currentMap);
        pthread_mutex_unlock(&mutexes.players);

        // Start game thread
        GameThreadArg arg = {
                .game = game,
                .maps = map,
                .settings = &gameSettings,
                .mutexes = &mutexes
        };
        pthread_t id;
        pthread_create(&id, NULL, gameThread, &arg );

        // read commands thread

        pthread_join(id, NULL);
        printf("\n\nA iniciar nova ronda!\n");
    }


    char adminCommand[50];
    while (1) {

        printf("\nInsira um comando do administrador: ");
        fgets(adminCommand, sizeof(adminCommand), stdin);
        printf("\n");
        adminCommand[strcspn(adminCommand, "\n")] = '\0';  // Remova a nova linha

        // Processa o comando do administrador
        if( processAdminCommand(adminCommand, &gameSettings) == -1)
            break;
    }

    terminate(0);
    return 0;
}

