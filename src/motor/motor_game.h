#ifndef SO_TP_MOTOR_GAME_H
#define SO_TP_MOTOR_GAME_H

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

#include "../common/common_data_structs.h"
#include "../common/constants.h"
#include "../common/messages.h"
#include "../common/utils.h"
#include "data_structs.h"



void* gameThread(void* arg);


typedef struct {
    Game* game;
    Map* maps;
    GameSettings* settings;
} GameThreadArg;


#endif //SO_TP_MOTOR_GAME_H
