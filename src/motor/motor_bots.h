#ifndef SO_TP_MOTOR_BOTS_H
#define SO_TP_MOTOR_BOTS_H


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "../common/constants.h"
#include "../common/messages.h"
#include "../common/utils.h"
#include "data_structs.h"
#include "motor_backend.h"


void runBots(Game *game, Map *currentMap, Mutexes *mutexes);


typedef struct {
    Bot* thisBot;
    Map* currentMap;
    Mutexes* mutexes;
    Player* players;
    int nPlayers;
} BotThreadArg;


#endif //SO_TP_MOTOR_BOTS_H
