#ifndef SO_TP_MOTOR_ADMINCONSOLE_H
#define SO_TP_MOTOR_ADMINCONSOLE_H


#include <signal.h>
#include <pthread.h>


#include "../common/common_data_structs.h"
#include "../common/messages.h"
#include "../common/constants.h"
#include "../common/utils.h"
#include "data_structs.h"
#include "motor_backend.h"


void* commandsInputThread(void* arg);



typedef struct {
    Game* game;
    Mutexes* mutexes;
} CommandsInputThreadArg;


typedef struct {
    MBlock *thisMBlock;
    Map *currentMap;
    Mutexes* mutexes;
    Player* players;
    int* nPlayers;
} MobileBlockThreadArg;

#endif
