#ifndef SO_TP_MOTOR_BACKEND_H
#define SO_TP_MOTOR_BACKEND_H


#include <pthread.h>

#include "../common/utils.h"
#include "data_structs.h"

void broadcastMessageToPlayers(Player players[], int nPlayers, int msgType, void * msg, int msgSize,
                               pthread_mutex_t * playersMutex);

#endif //SO_TP_MOTOR_BACKEND_H
