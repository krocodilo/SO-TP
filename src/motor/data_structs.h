#ifndef SO_TP_DATA_STRUCTS_H
#define SO_TP_DATA_STRUCTS_H


#include "../common/constants.h"


typedef struct {
    char username[MAX_PLAYER_NAME];
    int fd_pipe;
} Player;


typedef struct {
    Player players[MAX_PLAYERS];
    int nPlayers;
} Game;


typedef struct {
    int signupWindowDurationSeconds;
    int firstLevelDurationSeconds;
    int levelDurationDecreaseSeconds;
    int minPlayers;
} GameSettings;

#endif
