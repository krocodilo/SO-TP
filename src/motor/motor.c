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
#include "motor_signup.h"
#include "motor_init.h"
#include "motor_game.h"
#include "motor_bots.h"
#include "motor_adminConsole.h"


Game* game;
Map allMaps[MAX_LEVELS];


void terminate(int exitcode){
    fflush(stdout);
    printf("\nTerminating...\n");

    // wait for all threads to join

    // Terminate all bots
    for( ;game->nBots > 0; game->nBots-- ){
        kill(game->bots[game->nBots-1].pid, SIGINT);
//        wait(???); // esperar que os bots terminem
    }

    // when using mutexes, wait to lock and then unlock

    // Close general pipe
    if(game->generalPipe > 0)
        close(game->generalPipe);
    unlink(GENERAL_PIPE);

    // Close client pipes

    free(game);
    fflush(stdout);
    exit(exitcode);
}


int main(int argc, char *argv[]) {
    GameSettings gameSettings;

    // Inicializar o gerador de números aleatórios com uma semente
    srand((unsigned int)time(NULL));


//    Map map;
//    Position pos = {1,1};
//    int it = 0;
////    for(; it < 30; it++){
////        printf("%d\n", drand48() < 0.5);
////    }
//    do {
//        // chose horizontal or vertical direction:
//        if(drand48() < 0.5){
//            // if horizontal
//            if(drand48() < 0.5)
//                pos.x -= 1;    // left
//            else
//                pos.x += 1;    // right
//        }
//        else{
//            // if vertical
//            if(drand48() < 0.5)
//                pos.y -= 1;    // up
//            else
//                pos.y += 1;    // down
//        }
//        printf("y:%d\tx:%d\n", pos.y, pos.x);
//        fflush(stdout);
//        it++;
//    } while ((!posIsValid(pos) ) && it < 30);
//    printf("yoyoyo\n");
//
//
//    exit(1);




    // Read all allMaps
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

    // Initialize mutexes
    Mutexes mutexes;
    pthread_mutex_init(&mutexes.currentMap, NULL);
    pthread_mutex_init(&mutexes.players, NULL);
    pthread_mutex_init(&mutexes.bots, NULL);

    while(true){
        if( waitForClientsSignUp(gameSettings, game) == EXIT_FAILURE) {
            terminate(EXIT_FAILURE);
        }

        // Start game thread
        GameThreadArg gameArgs = {
                .game = game,
                .maps = allMaps,
                .settings = &gameSettings,
                .mutexes = &mutexes
        };
        pthread_t gameThreadId;
        pthread_create(&gameThreadId, NULL, gameThread, &gameArgs );

        // Start admin console thread

        CommandsInputThreadArg consoleArgs = {
                .game = game,
                .mutexes = &mutexes
        };
        pthread_t consoleThreadId;
        pthread_create(&consoleThreadId, NULL, commandsInputThread, &consoleArgs );

        pthread_join(gameThreadId, NULL);

        // end commands thread
        pthread_kill(consoleThreadId, SIGTERM);
        pthread_join(consoleThreadId, NULL);

        printf("\n\nA iniciar nova ronda!\n");
        resetGame(game);
    }

    terminate(0);
    return 0;
}

