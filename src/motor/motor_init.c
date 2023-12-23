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




// Função para inicializar o personagem
Character initCharacter(int x, int y, char symbol) {
    Character character;
    character.x = x;
    character.y = y;
    character.symbol = symbol;
    return character;
}

// Função para inicializar as coordenadas x e y dos jogadores
void initializePlayers(Player players[], int numPlayers) {
    for (int i = 0; i < numPlayers; i++) {
        // Inicializa as coordenadas x para cada jogador com valores aleatórios entre 3 e 37
        players[i].info = initCharacter(rand() % (37 - 3 + 1) + 3, 0, 'P'); // 'P' é apenas um exemplo
        // Define o símbolo como o primeiro caractere no campo username
        players[i].username[0] = players[i].info.symbol;
        // Os caracteres restantes no campo username podem ser definidos conforme necessário
    }
}