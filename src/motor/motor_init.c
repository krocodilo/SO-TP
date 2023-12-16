#include "motor_init.h"





void resetGame(Game* game) {
    game->nPlayers = 0;
    game->nBots = 0;
    game->generalPipe = 0;
}


int readEnvironmentVariables(GameSettings* gs){
    char* envvar;

    envvar = getenv(ENV_SIGNUP_WINDOW_DURATION_SECONDS);
    if(envvar == NULL){
        fprintf(stderr, "\nERRO: impossivel ler variavel de ambiente %s\n", ENV_SIGNUP_WINDOW_DURATION_SECONDS);
        return EXIT_FAILURE;
    }
    gs->signupWindowDurationSeconds = atoi(envvar);
    if(gs->signupWindowDurationSeconds <= 0){
        fprintf(stderr, "\nERRO: valor incorreto na variavel de ambiente %s\n", ENV_SIGNUP_WINDOW_DURATION_SECONDS);
        return EXIT_FAILURE;
    }

    //
    envvar = getenv(ENV_FIRST_LEVEL_DURATION_SECONDS);
    if(envvar == NULL){
        fprintf(stderr, "\nERRO: impossivel ler variavel de ambiente %s\n", ENV_FIRST_LEVEL_DURATION_SECONDS);
        return EXIT_FAILURE;
    }
    gs->firstLevelDurationSeconds = atoi(envvar);
    if(gs->firstLevelDurationSeconds <= 0){
        fprintf(stderr, "\nERRO: valor incorreto na variavel de ambiente %s\n", ENV_FIRST_LEVEL_DURATION_SECONDS);
        return EXIT_FAILURE;
    }

    //
    envvar = getenv(ENV_LEVEL_DURATION_DECREASE_SECONDS);
    if(envvar == NULL){
        fprintf(stderr, "\nERRO: impossivel ler variavel de ambiente %s\n", ENV_LEVEL_DURATION_DECREASE_SECONDS);
        return EXIT_FAILURE;
    }
    gs->levelDurationDecreaseSeconds = atoi(envvar);
    if(gs->levelDurationDecreaseSeconds <= 0){
        fprintf(stderr, "\nERRO: valor incorreto na variavel de ambiente %s\n", ENV_LEVEL_DURATION_DECREASE_SECONDS);
        return EXIT_FAILURE;
    }

    //
    envvar = getenv(ENV_MIN_PLAYERS);
    if(envvar == NULL){
        fprintf(stderr, "\nERRO: impossivel ler variavel de ambiente %s\n", ENV_MIN_PLAYERS);
        return EXIT_FAILURE;
    }
    gs->minPlayers = atoi(envvar);
    if(gs->minPlayers < 2){
        fprintf(stderr, "\nERRO: valor incorreto na variavel de ambiente %s. Tem de ser superior a 1.\n", ENV_MIN_PLAYERS);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}