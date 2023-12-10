#ifndef SO_TP_CONSTANTS_H
#define SO_TP_CONSTANTS_H



// Game Limits constants
#define MAX_PLAYERS     5
#define MAX_BOTS        10
#define MAX_PEDRAS      50  // maximo de pedras presentes no mapa em simultaneo
#define MAX_LEVELS      3
#define MAX_BLOQ_MOVEIS 5   // maximo bloqueios moveis


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
#define MAX_BOT_NAME 30

#define GENERAL_PIPE "/tmp/TP_SO_2023_24_GENERAL_PIPE"



#endif
