#ifndef SO_TP_MOTOR_SIGNUP_H
#define SO_TP_MOTOR_SIGNUP_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

#include "data_structs.h"
#include "../common/constants.h"
#include "../common/messages.h"

#include "global_variables.h"
Game* game;     // External global variable, declared in global_variables.h


void waitForClientsSignUp(GameSettings gameSettings);
bool receiveNewPlayer(Player* newPlayer);



#endif //SO_TP_MOTOR_SIGNUP_H
