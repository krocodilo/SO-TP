#ifndef SO_TP_MOTOR_SIGNUP_H
#define SO_TP_MOTOR_SIGNUP_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>

#include "data_structs.h"
#include "../common/constants.h"
#include "../common/messages.h"
#include "../common/utils.h"
#include "motor_backend.h"


int waitForClientsSignUp(GameSettings gameSettings, Game* game, Mutexes *mutx);


#endif //SO_TP_MOTOR_SIGNUP_H
