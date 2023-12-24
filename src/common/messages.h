#ifndef SO_TP_MESSAGES_H
#define SO_TP_MESSAGES_H

#include "common_data_structs.h"
#include "constants.h"


enum MessageTypes {
    // Sent by motor:
    SignUpSuccessful,   // Sends no additional message
    NewLevel,
    ModifyMap,
    PlayersList,
    Terminate,          // Sends GenericMessage


    // Sent by jogoUI:
    SignUp,
    Move,
    GetPlayersList,     // Sends no additional message
    LeaveGame,          // Sends no additional message
    TextMsg
};


////////////////////////////////////////////////////
// Sent by motor:

typedef struct {
    char message[MAX_MESSAGE_SIZE];
} GenericMessage;

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


/////////////////////////////////////////////////
// Sent by jogoUI:

typedef struct {
    int pid;
    char username[MAX_PLAYER_NAME];
    char pipePath[PIPE_PATH_MAX_SIZE];
} SignUpMessage;

typedef struct {
    int pid;
    int x, y;
} MoveMessage;

typedef struct {
    char from[MAX_PLAYER_NAME];
    char message[MAX_MESSAGE_SIZE];
} TextMessage;


////////////////////////////////////////////////////

#endif
