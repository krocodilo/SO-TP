#ifndef SO_TP_DATA_STRUCTS_H
#define SO_TP_DATA_STRUCTS_H


#include "constants.h"


typedef struct {
    int id;
} Player;


typedef struct {
    Player players[MAX_PLAYERS];
    int nPlayers;
} Game;

#endif
