#ifndef SO_TP_DATA_STRUCTS_H
#define SO_TP_DATA_STRUCTS_H


#include "../common/constants.h"

typedef struct {
    int x, y;
} MobileBlock;


typedef struct {
    int x, y, durationSeconds;
} Rock;


typedef struct {
    char name[MAX_BOT_NAME];
    pid_t pid;
} Bot;

typedef struct {
    char username[MAX_PLAYER_NAME];
    int pipe;
} Player;


typedef struct {
    Player players[MAX_PLAYERS];
    Bot bots[MAX_BOTS];
    int generalPipe; // file descriptor for general pipe
    int nPlayers;
    int nBots;
    int currentLevel;
    int nBlocks;
    int nRocks;
} Game;


typedef struct {
    int signupWindowDurationSeconds;
    int firstLevelDurationSeconds;
    int levelDurationDecreaseSeconds;
    int minPlayers;
} GameSettings;


typedef struct {
    char cmap[MAP_LINES][MAP_COLS + 1];
} Map;

typedef struct{
    int x;
    int y;
    char symbol;
} Character;



#endif
