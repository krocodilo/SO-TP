#include "motor_game.h"




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

//Função para verificar se há colisão na próxima posição
bool verificaColisao(char mapa[][MAP_COLS + 1], int nextY, int nextX) {
    // Ajusta as coordenadas do jogador para corresponder ao mapeamento no mapa
    int playerMapY = nextY - 1;
    int playerMapX = nextX / 2;

    // Verifica se a próxima posição contém 'X' (obstáculo)
    return mapa[playerMapY][playerMapX] == 'X';
}

// Send new map to all players
void broadcastNewMap(Game * game, Map * currentMap, Mutexes* mutx) {
    pthread_mutex_lock(&mutx->currentMap);

    NewLevelMessage mapMsg = {game->currentLevel};
    copyMap(&mapMsg.map, currentMap);
    broadcastMessageToPlayers(game, NewLevel, &mapMsg, sizeof(mapMsg), &mutx->players);

    pthread_mutex_unlock(&mutx->currentMap);
}



// Game Logic
void* gameThread(void* arg) {
    // Parse arguments into local pointers
    Game* game = (Game*) ((GameThreadArg*) arg)->game;
    Map* maps = (Map*) ((GameThreadArg*) arg)->maps;
    GameSettings* settings = (GameSettings*) ((GameThreadArg*) arg)->settings;
    Mutexes* mutx = (Mutexes*) ((GameThreadArg*) arg)->mutexes;

    Map currentMap;

    game->currentLevel = 1;
    while(game->currentLevel <= MAX_LEVELS ) {
        copyMap(&currentMap, &maps[game->currentLevel-1]);  // create a copy of this level's map, to then be modified
        printf("Iniciou o nivel %d", game->currentLevel);

        // Send new map
        broadcastNewMap(game, &currentMap, mutx);

        // Start bots



        sleep(100000000);
        game->currentLevel++;
    }
    return NULL;
}
