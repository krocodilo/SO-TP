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


void waitForClientsSignUp(GameSettings gameSettings, Game* game);
bool receiveNewPlayer(Player* newPlayer, int generalPipe);



#endif //SO_TP_MOTOR_SIGNUP_H
