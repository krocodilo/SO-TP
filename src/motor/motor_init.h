#ifndef SO_TP_MOTOR_INIT_H
#define SO_TP_MOTOR_INIT_H

#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include "data_structs.h"

int create_and_open(char* pipename, int flags);
void new_game(Game* game);
int readEnvironmentVariables(GameSettings* gs);

#endif
