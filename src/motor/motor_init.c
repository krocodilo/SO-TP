#include "motor_init.h"


int create_and_open(char* pipename, int flags)
{
    /*
    Recebe:
        -> string do nome do pipe a criar com permissoes 0777
        -> flags a passar a funcao open
    Devolve:
        file deescriptor do pipe em caso de sucesso
        -1  ->  caso nao consiga abrir o pipe criado
        -2  ->  caso ja exista um pipe com esse nome
        -3  ->  caso ocorra erro ao criar o pipe
    */
    //Cria o pipe generico
    if (mkfifo(pipename, 0666) == 0){
        //do nothing
    }else if (errno == EEXIST){
        // Ja existe um pipe com esse nome
        return -2;
    }else{
        // falha ao criar o pipe.
        return -3;
    }

    int pipe_fd = open(pipename, flags);
    if(pipe_fd == -1){
        // nao foi possivel abrir o pipe
        unlink(pipename);   //elimina o pipe
    }
    return pipe_fd;
}

//void new_game(Game* game) {
//    game->
//}


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