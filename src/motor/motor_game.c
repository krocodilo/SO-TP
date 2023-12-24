#include "motor_game.h"




//Função para verificar se há colisão na próxima posição
bool verificaColisao(char mapa[][MAP_COLS + 1], int nextY, int nextX) {
    // Ajusta as coordenadas do jogador para corresponder ao mapeamento no mapa
    int playerMapY = nextY - 1;
    int playerMapX = nextX / 2;

    // Verifica se a próxima posição contém 'X' (obstáculo)
    return mapa[playerMapY][playerMapX] == 'X';
}


void* gameThread(void* arg) {
    Game* game = (Game*) ((GameThreadArg*) arg)->game;
    Map* maps = (Map*) ((GameThreadArg*) arg)->maps;
    GameSettings* settings = (GameSettings*) ((GameThreadArg*) arg)->settings;

    game->currentLevel = 1;
    while(game->currentLevel <= MAX_LEVELS ) {
        printf("Iniciou o nivel %d", game->currentLevel);
        fflush(stdout);

        NewLevelMessage newMap = {game->currentLevel};
        memcpy(&newMap, &maps[game->currentLevel-1], sizeof(Map));

        for(int i = 0; i < game->nPlayers; i++)
            if( ! writeMessage(game->players[i].pipe, NewLevel, &newMap, sizeof(newMap)) ){
                fprintf(stderr, "Erro ao enviar mapa para %s.", game->players[i].username);
            }

        sleep(1000000000);
        game->currentLevel++;
    }
    return NULL;
}