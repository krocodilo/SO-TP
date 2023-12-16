#ifndef SO_TP_MESSAGES_H
#define SO_TP_MESSAGES_H

#include "constants.h"


enum MotorMessageTypes{
    // Sent by motor
    NewMap,
    ModifyMap,
};

enum JogoUIMessageTypes{
    // Sent by jogoUI
    Move,
};


typedef struct {
    char username[MAX_PLAYER_NAME];
    char pipePath[PIPE_PATH_MAX_SIZE];
} SignUpMessage;


#endif
