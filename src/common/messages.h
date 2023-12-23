#ifndef SO_TP_MESSAGES_H
#define SO_TP_MESSAGES_H

#include "common_data_structs.h"
#include "constants.h"


enum MessageTypes {
    // Sent by motor:
    NewLevel,
    ModifyMap,
    PlayersList,
    Terminate,


    // Sent by jogoUI:
    SignUp,
    Move,
    GetPlayersList,
    LeaveGame,


    // Common:
    TextMsg     // to be used by motor and to send messages between clients directly
};

////////////////////////////////////////////////////
// Sent by motor:


typedef struct {
    int level;
    Map map;
} NewLevelMessage;

typedef struct {
    int x, y;
    char newChar;
} ModifyMapMessage;

typedef struct {
    char players[MAX_PLAYERS][MAX_PLAYER_NAME];
} PlayersListMessage;

typedef struct {
    char from[MAX_PLAYER_NAME];
    char message[MAX_MESSAGE_SIZE];
} TextMessage;

////////////////////////////////////////////////////
// Sent by jogoUI:


typedef struct {
    char username[MAX_PLAYER_NAME];
    char pipePath[PIPE_PATH_MAX_SIZE];
} SignUpMessage;

typedef struct {
    int x, y;
} MoveMessage;



#endif
