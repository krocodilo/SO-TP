#include "motor_backend.h"



void broadcastMessageToPlayers(Game * game, int msgType, void * msg, int msgSize, pthread_mutex_t * playersMutex) {
    pthread_mutex_lock(playersMutex);
    for(int i = 0; i < game->nPlayers; i++)
        if( ! writeMessage(game->players[i].pipe, msgType, msg, msgSize) )
            fprintf(stderr, "Erro ao enviar mensagem #%d para %s.", msgType, game->players[i].username);
    pthread_mutex_unlock(playersMutex);
}