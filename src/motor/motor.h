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

#include "global_variables.h"
#include "data_structs.h"
#include "../common/constants.h"
#include "../common/utils.h"
#include "motor_init.h"
#include "motor_signup.h"

#define MAX_COMMAND_LENGTH 50


int exitcode = EXIT_SUCCESS;


Game* game;     // External global variable, declared in global_variables.h


#endif
