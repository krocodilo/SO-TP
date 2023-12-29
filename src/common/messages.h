#ifndef SO_TP_MESSAGES_H
#define SO_TP_MESSAGES_H

#include "common_data_structs.h"
#include "constants.h"


enum MessageTypes {
    // Sent by motor:
    SignUpSuccessful,   // Sends no additional message
    NewLevel,
    Move,
    ModifyMap,
    PlayersList,
    Terminate,          // Sends GenericMessage


    // Sent by jogoUI:
    SignUp,
    MoveRequest,
    GetPlayersList,     // Sends GenericRequest
    LeaveGame,          // Sends GenericRequest

    TextMsg             // Sent directly to other players
};


////////////////////////////////////////////////////
// Sent by motor:

typedef struct {
    char message[MAX_MESSAGE_SIZE];
} GenericMessage;

typedef struct {

} SignUpSuccessfulMessage;

typedef struct {
    int level;
    Map map;
} NewLevelMessage;

typedef struct {
    Position from;      // This position is to be freed
    Position to;
    char symbol;
} MoveMessage;

typedef struct {
    Position pos;
    char symbol;
} ModifyMapMessage;

typedef struct {
    char players[MAX_PLAYERS][MAX_PLAYER_NAME];
    int nPlayers;
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
    int arrowKey;
} MoveRequestMessage;

typedef struct {
    char from[MAX_PLAYER_NAME];
    char message[MAX_MESSAGE_SIZE];
} TextMessage;

typedef struct {
    int pid;
} GenericRequestMessage;


////////////////////////////////////////////////////

#endif
