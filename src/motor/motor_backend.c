#include "motor_backend.h"



void broadcastMessageToPlayers(Player players[], int nPlayers, int msgType, void * msg, int msgSize,
                               pthread_mutex_t * playersMutex) {
    pthread_mutex_lock(playersMutex);
    for(int i = 0; i < nPlayers; i++)
        if( ! writeMessage(players[i].pipe, msgType, msg, msgSize) )
            fprintf(stderr, "Erro ao enviar mensagem #%d para %s.", msgType, players[i].username);
    pthread_mutex_unlock(playersMutex);
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


bool posIsValid(Position pos) {
    return pos.y >= 0 && pos.y < MAP_LINES &&
           pos.x >= 0 && pos.x < MAP_COLS;
}

bool posIsFree(Position pos, Map *map) {
    return map->cmap[pos.y][pos.x] == FREE_SPACE;
}


void executeMoveRequest(Player *p, Map *currentMap, int arrowKey, Game *game, Mutexes *mutx) {
    pthread_mutex_lock(&mutx->players);
    Position pos = p->pos;
    pthread_mutex_unlock(&mutx->players);

    Position reqPos = pos;
    switch (arrowKey) {
        case KEY_UP: reqPos.y -= 1; break;
        case KEY_DOWN: reqPos.y += 1; break;
        case KEY_LEFT: reqPos.x -= 1; break;
        case KEY_RIGHT: reqPos.x += 1; break;
        default: return;
    }
    pthread_mutex_lock(&mutx->currentMap);
    // Check if position is valid and free
    if( ! posIsValid(reqPos) || ! posIsFree(reqPos, currentMap) ){
        pthread_mutex_unlock(&mutx->currentMap);
        printf("\n-> Player %s wanted to move to occupied position y:%d x:%d", p->username, reqPos.y, reqPos.x);
        fflush(stdout);
        return;
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
}


void handleNewGameMessage(Game *game, Map * currentMap, Mutexes *mutx) {

    switch ( readNextMessageType(game->generalPipe) ) {
        case MoveRequest: {
            MoveRequestMessage msg;
            if ( ! readNextMessage(game->generalPipe, &msg, sizeof(msg)) ) {
                perror("\nErro ao ler a proxima mensagem no pipe.");
                break;
            }
            // Get Player reference
            Player* player = getPlayerByPID(msg.pid, game->players, game->nPlayers, &mutx->players);
            if(player == NULL){
                fprintf(stderr, "\nERRO - recebeu mensagem de um jogador não registado. PID: %d.", msg.pid);
                break;
            }
            // Execute move, if position is free
            executeMoveRequest(player, currentMap, msg.arrowKey, game, mutx);
            break;
        }
        case GetPlayersList: {
            PlayersListMessage pMsg;
            pthread_mutex_lock(&mutx->players);

            pMsg.nPlayers = game->nPlayers;
            for(int i = 0; i < game->nPlayers; i++)
                strncpy(pMsg.players[i], game->players[i].username, MAX_PLAYER_NAME);

            pthread_mutex_unlock(&mutx->players);

            // Send changes to players
            broadcastMessageToPlayers(game->players, game->nPlayers, PlayersList, &pMsg, sizeof(pMsg), &mutx->players);
            break;
        }
        case LeaveGame: {
            break;
        }
        default:
            perror("\nErro ao ler o tipo da proxima mensagem no pipe. Tipo irreconhecível.");
    }
}

//Função para verificar se há colisão na próxima posição
//bool verificaColisao(char mapa[][MAP_COLS + 1], int nextY, int nextX) {
//    // Ajusta as coordenadas do jogador para corresponder ao mapeamento no mapa
//    int playerMapY = nextY - 1;
//    int playerMapX = nextX / 2;
//
//    // Verifica se a próxima posição contém 'X' (obstáculo)
//    return mapa[playerMapY][playerMapX] == 'X';
//}