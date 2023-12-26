#ifndef SO_TP_JOGOUI_BACKEND_H
#define SO_TP_JOGOUI_BACKEND_H


#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#include "../common/common_data_structs.h"
#include "../common/constants.h"
#include "../common/messages.h"
#include "../common/utils.h"

void* communicationsThread(void* arg);

typedef struct {
    int myPipe;
    Map * map;
} CommunicationsThreadArg;

#endif