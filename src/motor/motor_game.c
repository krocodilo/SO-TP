#include "motor_game.h"



void initializePlayerPositions(Player players[], int nPlayers, Map *currentMap, Mutexes *mutx) {
    pthread_mutex_lock(&mutx->players);
    pthread_mutex_lock(&mutx->currentMap);
    for (int i = 0; i < nPlayers;) {
        players[i].pos.y = rand() % (MAP_LINES + 1);
        players[i].pos.x = rand() % (MAP_COLS + 1);
        // Save, if position is free and valid
        if( posIsValid(players[i].pos) && posIsFree(players[i].pos, currentMap) ){
            currentMap->cmap[players[i].pos.y][players[i].pos.x] = players[i].symbol;
            i++;
        }
    }
    pthread_mutex_unlock(&mutx->currentMap);
    pthread_mutex_unlock(&mutx->players);
}


// Send new map to all players
void broadcastNewMap(int currentLevel, Player *players, int nPlayers, Map * currentMap, Mutexes* mutx) {
    pthread_mutex_lock(&mutx->currentMap);

    NewLevelMessage mapMsg = {currentLevel};
    copyMap(&mapMsg.map, currentMap);
    broadcastMessageToPlayers(players, nPlayers, NewLevel, &mapMsg, sizeof(mapMsg), &mutx->players);

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
    int levelDuration = settings->firstLevelDurationSeconds;

    game->currentLevel = 1;
    while(game->currentLevel <= MAX_LEVELS ) {
        levelDuration -= settings->levelDurationDecreaseSeconds;
        copyMap(&currentMap, &maps[game->currentLevel-1]);  // create a copy of this level's map, to then be modified
        printf("\nIniciou o nivel %d. Duração: %d\n", game->currentLevel, levelDuration);

        // Initialize player positions in current map
        initializePlayerPositions(game->players, game->nPlayers, &currentMap, mutx);

        // Send new map
        broadcastNewMap(game->currentLevel, game->players, game->nPlayers, &currentMap, mutx);
        printf("Novo mapa foi enviado aos jogadores.\n");

        // Run bot threads
        runBots(game, &currentMap, mutx);
        printf("Bots foram executados.\n");

        // Start game logic
        struct timeval waitTime = {.tv_sec = levelDuration};
        int pipesToWatch[1] = {game->generalPipe};
        fd_set selectHandler;
        while(true) {
//            printf("Time left: %ld\n", waitTime.tv_sec);
            if( selectPipe(&selectHandler, pipesToWatch, 1, &waitTime) == 0) {
                // Level Timeout
                break;
            }
            if(pipeIsSet(game->generalPipe, &selectHandler)) {
                // If there's something to read
                handleNewGameMessage(game, &currentMap, mutx);
            }
        }
//        sleep(100000000);
//        printf("!!! STOPPED WAITING !!!!");

        // TODO kill bots
        game->currentLevel++;
    }
    return NULL;
}
