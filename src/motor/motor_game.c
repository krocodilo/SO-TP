#include "motor_game.h"



void initializePlayerPositions(Player players[], int nPlayers, Map *currentMap, Mutexes *mutx) {
    pthread_mutex_lock(&mutx->players);
    pthread_mutex_lock(&mutx->currentMap);
    for (int i = 0; i < nPlayers;) {
        players[i].pos.y = MAP_LINES-1; //rand() % (MAP_LINES + 1);
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


Game *signalHandlerGame;
Mutexes *signalHandlerMutexes;

void terminateGame() {

    terminateAllMBlocks(signalHandlerGame->mBlocks, &signalHandlerGame->nMBlocks, &signalHandlerMutexes->mBlocks);
    terminateAllBots(signalHandlerGame->bots, &signalHandlerGame->nBots, &signalHandlerMutexes->bots);
    pthread_exit(NULL);
}


// Game Logic
void* gameThread(void* arg) {
    // Parse arguments into local pointers
    Game* game = (Game*) ((GameThreadArg*) arg)->game;
    Map* allMaps = (Map*) ((GameThreadArg*) arg)->maps;
    GameSettings* settings = (GameSettings*) ((GameThreadArg*) arg)->settings;
    Mutexes* mutx = (Mutexes*) ((GameThreadArg*) arg)->mutexes;

    signal(SIGTERM, terminateGame);
    signalHandlerGame = game; signalHandlerMutexes = mutx;

    // Select variables
    struct timeval waitTime = {.tv_sec = -1};
    int pipesToWatch[1] = {game->generalPipe};
    fd_set selectHandler;

    TextMessage infoMsg = {.from = "Motor"};
    Map* currentMap = &game->currentMap;
    int levelDuration = settings->firstLevelDurationSeconds;

    game->currentLevel = 1;
    bool gameIsRunning = true;
    while( gameIsRunning && game->currentLevel <= MAX_LEVELS && game->nPlayers > 0 ) {

        levelDuration -= settings->levelDurationDecreaseSeconds;
        copyMap(currentMap, &allMaps[game->currentLevel-1]);  // create a copy of this level's map, to then be modified
        printf("\nIniciou o nivel %d. Duração: %ds\n", game->currentLevel, levelDuration);

        // Initialize player positions in current map
        initializePlayerPositions(game->players, game->nPlayers, currentMap, mutx);

        // Run bot threads
        runBots(game, currentMap, mutx);

        // Send new map
        broadcastNewMap(game->currentLevel, game->players, game->nPlayers, currentMap, mutx);
        printf("Novo mapa foi enviado aos jogadores.\n");

        // Start game logic
        bool levelIsWon = false;
        waitTime.tv_sec = levelDuration;    // Set level timeout
        printf("Motor esta a escuta de mensagens...\n");
        flushPipe(game->generalPipe);
        while( gameIsRunning && !levelIsWon && game->nPlayers > 0 ) {
//            printf("Time left: %ld\n", waitTime.tv_sec);
            if( selectPipe(&selectHandler, pipesToWatch, 1, &waitTime) == 0) {
                // Level Timeout
                printf("O nivel %d chegou ao tempo limite.\n", game->currentLevel);

                char string[MAX_MESSAGE_SIZE];
                snprintf(infoMsg.message, MAX_MESSAGE_SIZE, "O nivel %d chegou ao tempo limite.", game->currentLevel);
                broadcastMessageToPlayers(game->players, game->nPlayers, TextMsg, &infoMsg, sizeof(infoMsg), &mutx->players);
                break;
            }
            if(pipeIsSet(game->generalPipe, &selectHandler)) {
                // If there's something to read
                switch( handleNewGameMessage(game, currentMap, mutx) ) {
                    case 1:
                        levelIsWon = true;
                        continue;
                    case -1:
                        gameIsRunning = false;
                        continue;
                }
            }
        }
        terminateAllMBlocks(game->mBlocks, &game->nMBlocks, &mutx->mBlocks);
        terminateAllBots(game->bots, &game->nBots, &mutx->bots);

        game->currentLevel++;
    }

    terminateGame();
}