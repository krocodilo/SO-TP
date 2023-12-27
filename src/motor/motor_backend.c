#include "motor_backend.h"



void broadcastMessageToPlayers(Player players[], int nPlayers, int msgType, void * msg, int msgSize,
                               pthread_mutex_t * playersMutex) {
    pthread_mutex_lock(playersMutex);
    for(int i = 0; i < nPlayers; i++)
        if( ! writeMessage(players[i].pipe, msgType, msg, msgSize) )
            fprintf(stderr, "Erro ao enviar mensagem #%d para %s.", msgType, players[i].username);
    pthread_mutex_unlock(playersMutex);
}