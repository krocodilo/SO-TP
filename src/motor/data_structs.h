#ifndef SO_TP_DATA_STRUCTS_H
#define SO_TP_DATA_STRUCTS_H


#include "../common/constants.h"
#include "../common/common_data_structs.h"

typedef struct {
    int id, period, duration;
    pthread_t threadId;
    pid_t pid;
    int pipe;
} Bot;

typedef struct {
    pthread_t threadId;
    Position pos;
} MBlock;

typedef struct {
    char symbol;
    Position pos;
    int pid;
    char username[MAX_PLAYER_NAME];
    int pipe;
} Player;

typedef struct {
    Player players[MAX_PLAYERS];
    int nPlayers;
    Bot bots[MAX_BOTS];
    int nBots;
    MBlock mBlocks[MAX_MBLOCKS];
    int nMBlocks;
    Map currentMap;
    int currentLevel;
    int generalPipe; // file descriptor for general pipe
    int nRocks;
    pthread_t gameThreadId;
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
    pthread_mutex_t bots;
    pthread_mutex_t mBlocks;
} Mutexes;



#endif
