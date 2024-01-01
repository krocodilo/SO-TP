#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

#include "../common/constants.h"
#include "data_structs.h"
#include "motor_signup.h"
#include "motor_init.h"
#include "motor_game.h"
#include "motor_adminConsole.h"


Game* game = NULL;
Map allMaps[MAX_LEVELS];
Mutexes mutexes;


void terminate(int exitcode){
    printf("\nA terminar...  ");
    fflush(stdout);

    if(game != NULL) {
        terminateAllPlayers(game->players, &game->nPlayers, &mutexes.players, "O Motor terminou.");

        if( game->gameThreadId != pthread_self() ){
            pthread_kill(game->gameThreadId, SIGTERM);
            pthread_join(game->gameThreadId, NULL);
        }

        // Close general pipe
        if(game->generalPipe > 0)
            close(game->generalPipe);
        unlink(GENERAL_PIPE);

        // Close client pipes
        for(int i = 0; i < game->nPlayers; i++)
            close(game->players[i].pipe);

        free(game);
    }

    printf("Terminou com sucesso!\n");
    fflush(stdout);
    exit(exitcode);
}



int main(int argc, char *argv[]) {
    GameSettings gameSettings;

    // Inicializar o gerador de números aleatórios com uma semente
    srand((unsigned int)time(NULL));


    // Read all currentMap
    if(readMapFiles(allMaps) == EXIT_FAILURE )
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

    // Initialize mutexes
    pthread_mutex_init(&mutexes.currentMap, NULL);
    pthread_mutex_init(&mutexes.players, NULL);
    pthread_mutex_init(&mutexes.bots, NULL);
    pthread_mutex_init(&mutexes.mBlocks, NULL);

    while(true){
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

        if( waitForClientsSignUp(gameSettings, game, &mutexes) == EXIT_FAILURE) {
            terminate(EXIT_FAILURE);
        }
        fflush(stdout);

        // Start game thread
        GameThreadArg gameArgs = {
                .game = game,
                .maps = allMaps,
                .settings = &gameSettings,
                .mutexes = &mutexes
        };
        pthread_create(&game->gameThreadId, NULL, gameThread, &gameArgs );

        // Start admin console thread
        CommandsInputThreadArg consoleArgs = {
                .game = game,
                .mutexes = &mutexes
        };
        pthread_t consoleThreadId;
        pthread_create(&consoleThreadId, NULL, commandsInputThread, &consoleArgs );

        pthread_join(game->gameThreadId, NULL);
        terminateAllPlayers(game->players, &game->nPlayers, &mutexes.players, "O jogo terminou.");

        // end commands thread
        pthread_kill(consoleThreadId, SIGTERM);
        void *res;
        pthread_join(consoleThreadId, &res);
//        if(res != NULL){
////            int resInt = (int)(intptr_t)res;
////            if( resInt == EXIT_SUCCESS)
//                terminate(EXIT_SUCCESS);
//        }

        printf("\n\nA iniciar nova ronda!\n");
        resetGame(game);
//        flushPipe(game->generalPipe);
        close(game->generalPipe);
        unlink(GENERAL_PIPE);
    }
    terminate(0);
    return 0;
}

