#include "motor_backend.h"


//bool sendMessageToPlayer(Player *p, int msgType, void * msg, int msgSize, pthread_mutex_t * playersMutex);
Player* getPlayerByPID(int pid, Player players[], int nPlayers, pthread_mutex_t *playersMutex);
int executeMoveRequest(Player *p, Map *currentMap, int arrowKey, Game *game, Mutexes *mutx);


int handleNewGameMessage(Game *game, Map * currentMap, Mutexes *mutx) {
    /*
     * Returns:
     *      1 - if a player has won the current level
     *      0 - if all is ok
     *      -1 - if there are no players left
     */

    switch ( readNextMessageType(game->generalPipe) ) {
        case MoveRequest: {
            MoveRequestMessage msg;
            if ( ! readNextMessage(game->generalPipe, &msg, sizeof(msg)) ) {
                perror("\nErro ao ler a proxima mensagem no pipe.");
                break;
            }
            // Get Player reference
            Player *player = getPlayerByPID(msg.pid, game->players, game->nPlayers, &mutx->players);
            if(player == NULL){
                fprintf(stderr, "\nERRO - recebeu mensagem de um jogador não registado. PID: %d.", msg.pid);
                break;
            }
            pthread_mutex_lock(&mutx->players);
            Position playerPos = player->pos;
            pthread_mutex_unlock(&mutx->players);

            // Execute move, if position is free
//            if( executeMoveRequest(player, currentMap, msg.arrowKey, game, mutx) == EXIT_SUCCESS ){

            if (playerPos.y == 0 && msg.arrowKey == KEY_UP) {
                // If player has won
                TextMessage infoMsg;
                strncpy(infoMsg.from, "Motor", MAX_PLAYER_NAME-1);

                pthread_mutex_lock(&mutx->players);
                snprintf(infoMsg.message, MAX_MESSAGE_SIZE-1, "Player '%s' has won level %d.", player->username, game->currentLevel);
                pthread_mutex_unlock(&mutx->players);

                // Inform all players
                broadcastMessageToPlayers(game->players, game->nPlayers, TextMsg, &infoMsg,
                                          sizeof(infoMsg), &mutx->players);
                return 1;
            }
            executeMoveRequest(player, currentMap, msg.arrowKey, game, mutx);
            break;
        }
        case GetPlayersList: {
            GenericRequestMessage msg;
            if ( ! readNextMessage(game->generalPipe, &msg, sizeof(msg)) ) {
                perror("\nErro ao ler a proxima mensagem no pipe.");
                break;
            }
            Player *player = getPlayerByPID(msg.pid, game->players, game->nPlayers, &mutx->players);
            if(player == NULL){
                fprintf(stderr, "\nERRO - recebeu mensagem de um jogador não registado. PID: %d.", msg.pid);
                break;
            }
            PlayersListMessage plMsg;
            pthread_mutex_lock(&mutx->players);

            plMsg.nPlayers = game->nPlayers;
            for(int i = 0; i < game->nPlayers; i++) {
                strncpy(plMsg.players[i], game->players[i].username, MAX_PLAYER_NAME);
            }
            // Send changes to players
            if( ! writeMessage(player->pipe, PlayersList, &plMsg, sizeof(plMsg)) )
                fprintf(stderr, "Erro ao enviar mensagem #%d para %s.", PlayersList, player->username);

//            sendMessageToPlayer(player, PlayersList, &pMsg, sizeof(pMsg), &mutx->players);
            pthread_mutex_unlock(&mutx->players);
            break;
        }
        case LeaveGame: {
            GenericRequestMessage msg;
            if ( ! readNextMessage(game->generalPipe, &msg, sizeof(msg)) ) {
                perror("\nErro ao ler a proxima mensagem no pipe.");
                break;
            }
            Player *player = getPlayerByPID(msg.pid, game->players, game->nPlayers, &mutx->players);
            if(player == NULL){
                fprintf(stderr, "\nERRO - recebeu mensagem de um jogador não registado. PID: %d.", msg.pid);
                break;
            }
            TextMessage infoMsg;
            strncpy(infoMsg.from, "Motor", MAX_PLAYER_NAME-1);

            pthread_mutex_lock(&mutx->players);
            snprintf(infoMsg.message, MAX_MESSAGE_SIZE-1, "Player %s has left the game.", player->username);
            Position playerPos = player->pos;
            pthread_mutex_unlock(&mutx->players);

            // Inform all players
            broadcastMessageToPlayers(game->players, game->nPlayers, TextMsg, &infoMsg,
                                      sizeof(infoMsg), &mutx->players);
            // Update all maps
            ModifyMapMessage modMsg = {playerPos, FREE_SPACE};
            broadcastMessageToPlayers(game->players, game->nPlayers, ModifyMap, &modMsg,
                                      sizeof(modMsg), &mutx->players);
            // Remove player
            removePlayer(player, game->players, &game->nPlayers, &mutx->players);

            if( game->nPlayers == 0 ){
                printf("\nAll players have left the game.");
                return -1;
            }
            break;
        }
        case SignUp:{
            SignUpMessage msg;
            readNextMessage(game->generalPipe, &msg, sizeof(msg));
            // ignore
            break;
        }
        default:
            perror("\nErro ao ler o tipo da proxima mensagem no pipe. Tipo irreconhecível.");
    }
    return 0;
}


void broadcastMessageToPlayers(Player players[], int nPlayers, int msgType, void * msg, int msgSize,
                               pthread_mutex_t * playersMutex) {
    pthread_mutex_lock(playersMutex);
    for(int i = 0; i < nPlayers; i++)
        if( ! writeMessage(players[i].pipe, msgType, msg, msgSize) )
            fprintf(stderr, "Erro ao enviar mensagem #%d para %s.", msgType, players[i].username);
    pthread_mutex_unlock(playersMutex);
}


bool sendMessageToPlayer(Player *p, int msgType, void * msg, int msgSize, pthread_mutex_t * playersMutex) {
//    pthread_mutex_lock(playersMutex);
//
//    pthread_mutex_unlock(playersMutex);
}


Player* getPlayerByPID(int pid, Player players[], int nPlayers, pthread_mutex_t *playersMutex) {
    Player* ptr = NULL;
    pthread_mutex_lock(playersMutex);
    for(int i = 0; i < nPlayers; i++)
        if(players[i].pid == pid)
            ptr = &players[i];
    pthread_mutex_unlock(playersMutex);
    return ptr;
}


Player* getPlayerByUsername(char *string, Player players[], int nPlayers, pthread_mutex_t *playersMutex) {
    Player* ptr = NULL;
    pthread_mutex_lock(playersMutex);
    for(int i = 0; i < nPlayers; i++)
        if( strncmp(players[i].username, string, MAX_PLAYER_NAME) == 0 )
            ptr = &players[i];
    pthread_mutex_unlock(playersMutex);
    return ptr;
}


void removePlayer(Player *p, Player players[], int *nPlayers, pthread_mutex_t *playersMutex) {
    pthread_mutex_lock(playersMutex);
    if (p->pid != players[*nPlayers-1].pid) {
        // If player to be removed is not the last in the array
        bool shiftBack = false;
        for(int i = 0; i < *nPlayers; i++){
            if (players[i].pid == p->pid){
                shiftBack = true;
                continue;
            }
            if (shiftBack == true)
                // Moves element on current index to previous index
                memmove(&players[i-1], &players[i], sizeof(Player));
        }
    }
    // If player to be removed is the last, then simply decrese counter of players
    *nPlayers--;
    pthread_mutex_unlock(playersMutex);
}


bool posIsValid(Position pos) {
    return pos.y >= 0 && pos.y < MAP_LINES &&
           pos.x >= 0 && pos.x < MAP_COLS;
}

bool posIsFree(Position pos, Map *map) {
    return map->cmap[pos.y][pos.x] == FREE_SPACE;
}


int executeMoveRequest(Player *p, Map *currentMap, int arrowKey, Game *game, Mutexes *mutx) {
    /*
     * Returns:
     *       EXIT_SUCCESS - if the player has won the level
     */
    pthread_mutex_lock(&mutx->players);
    Position pos = p->pos;
    pthread_mutex_unlock(&mutx->players);

    Position reqPos = pos;
    switch (arrowKey) {
        case KEY_UP: reqPos.y -= 1; break;
        case KEY_DOWN: reqPos.y += 1; break;
        case KEY_LEFT: reqPos.x -= 1; break;
        case KEY_RIGHT: reqPos.x += 1; break;
        default: return EXIT_FAILURE;
    }
    if( ! posIsValid(reqPos) ){
        if( reqPos.y < 0 )
            return EXIT_SUCCESS;    // Player has won
        return EXIT_FAILURE;    // Invalid requested position
    }

    pthread_mutex_lock(&mutx->currentMap);
    // Check if position is valid and free
    if( ! posIsFree(reqPos, currentMap) ){
        pthread_mutex_unlock(&mutx->currentMap);
//        printf("\n-> Player '%s' wanted to move to occupied position y:%d x:%d", p->username, reqPos.y, reqPos.x);
//        fflush(stdout);
        return EXIT_FAILURE;
    }

    // Update map
    currentMap->cmap[reqPos.y][reqPos.x] = p->symbol;
    currentMap->cmap[pos.y][pos.x] = FREE_SPACE;
    pthread_mutex_unlock(&mutx->currentMap);

    // Update player position
    pthread_mutex_lock(&mutx->players);
    p->pos = reqPos;
    MoveMessage msg = {
            .from = pos,
            .to = reqPos,
            .symbol = p->symbol
    };
    pthread_mutex_unlock(&mutx->players);

    // Send changes to players
    broadcastMessageToPlayers(game->players, game->nPlayers, Move, &msg, sizeof(msg), &mutx->players);

    return EXIT_FAILURE;
}


Position randomFreePosition(Map *map, pthread_mutex_t *mapMutex) {
    Position pos;
    pthread_mutex_lock(mapMutex);
    while(true) {
        pos.y = rand() % (MAP_LINES + 1);
        pos.x = rand() % (MAP_COLS + 1);
        if( posIsValid(pos) && posIsFree(pos, map) )
            break;
    }
    pthread_mutex_unlock(mapMutex);
    return pos;
}

void terminateAllMBlocks(MBlock mBlocks[], int *nMBlocks, pthread_mutex_t *mBlocksMutex) {
//    printf("Terminating mblocks\n");
//    fflush(stdout);
    pthread_mutex_lock(mBlocksMutex);
    for(int i = 0; i < *nMBlocks; i++)
        pthread_kill(mBlocks[i].threadId, SIGTERM);
    for(int i = 0; i < *nMBlocks; i++)
        pthread_join(mBlocks[i].threadId, NULL);
    *nMBlocks = 0;
    pthread_mutex_unlock(mBlocksMutex);
}

void terminateAllBots(Bot bots[], int *nBots, pthread_mutex_t *botsMutex) {
    printf("Terminating bots\n");
    fflush(stdout);
    pthread_mutex_lock(botsMutex);
    for(int i = 0; i < *nBots; i++){
        pthread_kill(bots[i].threadId, SIGTERM);
        kill(bots[i].pid, SIGINT);
    }
    for(int i = 0; i < *nBots; i++)
        pthread_join(bots[i].threadId, NULL);
    *nBots = 0;
    pthread_mutex_unlock(botsMutex);
}

void terminateAllPlayers(Player players[], int *nPlayers, pthread_mutex_t *playersMutex, char *message) {
    TextMessage msg = {
            .from = "Motor"
    };
    strncpy(msg.message, message, MAX_MESSAGE_SIZE);
    broadcastMessageToPlayers(players, *nPlayers, TextMsg, &msg, sizeof(msg), playersMutex);

    int terminateMsgType = Terminate;
    pthread_mutex_lock(playersMutex);
    for(int i = 0; i < *nPlayers; i++)

        if( write(players[i].pipe, &terminateMsgType, sizeof(int)) != sizeof(int) )
            fprintf(stderr, "Erro ao enviar mensagem #%d para %s.", terminateMsgType, players[i].username);

    *nPlayers = 0;
    pthread_mutex_unlock(playersMutex);
}