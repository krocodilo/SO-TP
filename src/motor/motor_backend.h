#ifndef SO_TP_MOTOR_BACKEND_H
#define SO_TP_MOTOR_BACKEND_H

#include <string.h>
#include <ncurses.h>
#include <signal.h>
#include <pthread.h>

#include "../common/utils.h"
#include "../common/common_data_structs.h"
#include "../common/messages.h"
#include "data_structs.h"


int handleNewGameMessage(Game *game, Map * currentMap, Mutexes *mutx);
void broadcastMessageToPlayers(Player players[], int nPlayers, int msgType, void * msg, int msgSize,
                               pthread_mutex_t * playersMutex);
bool posIsValid(Position pos);
bool posIsFree(Position pos, Map *map);
Position randomFreePosition(Map *map, pthread_mutex_t *mapMutex);

void terminateAllMBlocks(MBlock mBlocks[], int *nMBlocks, pthread_mutex_t *mBlocksMutex);
void terminateAllBots(Bot bots[], int *nBots, pthread_mutex_t *botsMutex);
void terminateAllPlayers(Player players[], int *nPlayers, pthread_mutex_t *playersMutex, char *message);


#endif //SO_TP_MOTOR_BACKEND_H
