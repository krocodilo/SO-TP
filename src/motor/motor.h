#ifndef SO_TP_MOTOR_H
#define SO_TP_MOTOR_H

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
#include <sys/wait.h>
#include <pthread.h>

#include "data_structs.h"
#include "../common/constants.h"
#include "../common/utils.h"
#include "motor_init.h"
#include "motor_signup.h"

#define MAX_COMMAND_LENGTH 50


Game* game;

Map map[MAX_LEVELS];


#endif
