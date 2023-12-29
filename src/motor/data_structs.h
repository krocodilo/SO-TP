#ifndef SO_TP_DATA_STRUCTS_H
#define SO_TP_DATA_STRUCTS_H


#include "../common/constants.h"
#include "../common/common_data_structs.h"

typedef struct {
    int x, y;
} MobileBlock;


typedef struct {
    int x, y, durationSeconds;
} Rock;


typedef struct {
    int id, period, duration;
    pthread_t threadId;
    pid_t pid;
    int pipe;
} Bot;

typedef struct {
    char symbol;
    Position pos;
    int pid;
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
    pthread_mutex_t currentMap;
    pthread_mutex_t players;
} Mutexes;



#endif
