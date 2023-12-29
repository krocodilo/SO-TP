#ifndef SO_TP_MOTOR_BACKEND_H
#define SO_TP_MOTOR_BACKEND_H

#include <string.h>
#include <ncurses.h>
#include <pthread.h>

#include "../common/utils.h"
#include "../common/common_data_structs.h"
#include "../common/messages.h"
#include "data_structs.h"

void broadcastMessageToPlayers(Player players[], int nPlayers, int msgType, void * msg, int msgSize,
                               pthread_mutex_t * playersMutex);
bool posIsValid(Position pos);
bool posIsFree(Position pos, Map *map);
void handleNewGameMessage(Game *game, Map * currentMap, Mutexes *mutx);



#endif //SO_TP_MOTOR_BACKEND_H
