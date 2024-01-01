#ifndef SO_TP_COMMON_DATA_STRUCTS_H
#define SO_TP_COMMON_DATA_STRUCTS_H



#include "constants.h"

typedef struct {
    char cmap[MAP_LINES][MAP_COLS + 1];
} Map;


typedef struct {
    int x;
    int y;
} Position;


#endif //SO_TP_COMMON_DATA_STRUCTS_H
