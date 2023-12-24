#include "motor_init.h"





void resetGame(Game* game) {
    game->nPlayers = 0;
    game->nBots = 0;
    game->generalPipe = 0;
    game->currentLevel = 1;
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
    if(gs->minPlayers < 1){
        fprintf(stderr, "\nERRO: valor incorreto na variavel de ambiente %s. Tem de ser superior a 0.\n", ENV_MIN_PLAYERS);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


bool isLinhaEmBranco(const char *linha) {
    while (*linha) {
        if (!isspace(*linha)) {
            return false;  // A linha não está em branco
        }
        linha++;
    }
    return true;  // A linha está em branco
}



int readMapFiles(Map maps[]) {
    char filepath[100], filename[20];

    for(int i = 0; i < MAX_LEVELS; i++){

        snprintf(filename, sizeof(filename), "labN%d.txt", i+1);
        snprintf(filepath, sizeof(filepath), "./maps/%s", filename);

        FILE *filePointer;
        filePointer = fopen(filepath, "r");

        if (filePointer == NULL) {
            fprintf(stderr, "Erro ao abrir o ficheiro %s", filepath);
            return EXIT_FAILURE;
        }

        int linhaAtual = 0;
        char buffer[MAP_COLS + 2];  // +2 para incluir espaço para '\n' e '\0'

        while (linhaAtual < MAP_LINES && fgets(buffer, sizeof(buffer), filePointer) != NULL) {
            size_t comprimento = strlen(buffer);

            // Remover o caractere de nova linha, se existir
            if (comprimento > 0 && buffer[comprimento - 1] == '\n') {
                buffer[comprimento - 1] = '\0';
            }

            // Verificar se a linha não está em branco
            if (!isLinhaEmBranco(buffer)) {
                strncpy(maps[i].cmap[linhaAtual], buffer, MAP_COLS);
                linhaAtual++;
            }
        }

        fclose(filePointer);
    }
    return EXIT_SUCCESS;
}