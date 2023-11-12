#ifndef SO_TP_DATA_STRUCTS_H
#define SO_TP_DATA_STRUCTS_H


#include "../common/constants.h"

typedef struct {
    int x;
    int y;
} MobileBlock;

typedef struct {
    char name[MAX_BOT_NAME];
} Bot;

typedef struct {
    char username[MAX_PLAYER_NAME];
    int fd_pipe;
} Player;


typedef struct {
    Player players[MAX_PLAYERS];
    Bot bots[MAX_BOTS];
    int nPlayers;
    int nBots;
    int pipegen_fd; // file descriptor for general pipe
} Game;


typedef struct {
    int signupWindowDurationSeconds;
    int firstLevelDurationSeconds;
    int levelDurationDecreaseSeconds;
    int minPlayers;
} GameSettings;

#endif
