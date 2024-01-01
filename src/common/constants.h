#ifndef SO_TP_CONSTANTS_H
#define SO_TP_CONSTANTS_H


// UI Symbols
#define FREE_SPACE      ' '
#define CHAR_WALLS      'X'
#define CHAR_ROCK       'P'
#define CHAR_MBLOCKS    'B'


// Game Limits constants
#define MAX_PLAYERS     5
#define MAX_BOTS        10
#define MAX_ROCKS       50  // maximo de pedras presentes no mapa em simultaneo
#define MAX_LEVELS      3
#define MAX_MBLOCKS     5   // maximo bloqueios moveis


// Map size
#define MAP_LINES   16
#define MAP_COLS    40


// Environment Variables
#define ENV_SIGNUP_WINDOW_DURATION_SECONDS  "INSCRICAO"
#define ENV_FIRST_LEVEL_DURATION_SECONDS    "DURACAO"
#define ENV_LEVEL_DURATION_DECREASE_SECONDS "DECREMENTO"
#define ENV_MIN_PLAYERS                     "NPLAYERS"


// Additional Constants
#define MAX_PLAYER_NAME 30
#define MAX_COMMAND_LENGTH 50


// Pipes
#define PIPE_DIRECTORY "/tmp/"
#define GENERAL_PIPE "/tmp/TP_SO_2023_24_GENERAL_PIPE"
#define PIPE_PATH_MAX_SIZE (MAX_PLAYER_NAME + sizeof(PIPE_DIRECTORY))


// ...
#define MAX_MESSAGE_SIZE 256


#endif
