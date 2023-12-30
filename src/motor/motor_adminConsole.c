#include "motor_adminConsole.h"


void* mobileBlockThread(void* arg);


void* commandsInputThread(void* arg) {
    Game* game = (Game *) ((CommandsInputThreadArg*) arg)->game;
    Mutexes* mutx = (Mutexes *) ((CommandsInputThreadArg*) arg)->mutexes;

    char command[MAX_COMMAND_LENGTH];

    while( true ){
        printf("\n>  ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';  // Remove new line

        if (strcmp(command, "users") == 0) {
            printf("\n");
            pthread_mutex_lock(&mutx->players);
            for(int i = 0; i < game->nPlayers; i++)
                printf("- %s \t|\t char: %c \t|\t y:%d x:%d \t|\t pid: %d\n", game->players[i].username,
                       game->players[i].symbol, game->players[i].pos.y, game->players[i].pos.x, game->players[i].pid);
            pthread_mutex_unlock(&mutx->players);
        }

        else if (strncmp(command, "kick", 4) == 0) {

            char *token = strtok(command, " ");
            char *targetName = strtok(NULL, " ");

            if (targetName != NULL) {

                char *aux;
                aux = (char *)malloc(strlen(targetName) + strlen(" banido") + 1);
                strcpy(aux,targetName);
                strcat(aux," banido");
                printf("\n");
                puts(aux);
                printf("\n");
                free(aux);
            }
            else {
                printf("\nkick <nome do jogador>\n");
            }
        }

        else if (strncmp(command, "bots", 4) == 0) {
            printf("\n");
            pthread_mutex_lock(&mutx->bots);
            for(int i = 0; i < game->nBots; i++)
                printf("- %d \t|\t pid: %d \t|\t period: %ds \t|\t duration: %ds\n", game->bots[i].id, game->bots[i].pid,
                       game->bots[i].period, game->bots[i].duration);
            pthread_mutex_unlock(&mutx->bots);
        }

        else if (strncmp(command, "bmov", 4) == 0) {

            MobileBlockThreadArg mBlockArg = {
                    .players = game->players,
                    .nPlayers = &game->nPlayers,
                    .currentMap = &game->currentMap,
                    .mutexes = mutx
            };
            pthread_create(&game->mBlocks[game->nMBlocks], NULL, mobileBlockThread, &mBlockArg );
            game->nMBlocks++;
        }

        else if (strncmp(command, "rbm", 3) == 0) {

            if(game->nMBlocks == 0)
                continue;

            pthread_kill(game->mBlocks[0], SIGTERM);
//            int position =

            for(int i = 1; i < game->nMBlocks; i++){
                // If number of mobile blocks is more than 1, shift all back
                memmove(&game->mBlocks[i-1], &game->mBlocks[i], sizeof(Player));
            }
            // If number of mobile blocks is only one, then simply decrease counter
            game->nMBlocks--;

            // Update clients
//            ModifyMap removeMBlockMsg;

            printf("\nFoi eliminado o bloqueio movel mais antigo.\n");
        }

        else if (strncmp(command, "end", 3) == 0) {
            return NULL;
        }
        else
            printf("Comando inválido.\n");
    }
}


//void mobileBlockThreadSignalHandler() {
//
//    return;
//}


void* mobileBlockThread(void* arg) {
    Map* currentMap = (Map *) ((MobileBlockThreadArg*) arg)->currentMap;
    Mutexes* mutx = (Mutexes *) ((MobileBlockThreadArg*) arg)->mutexes;
    Player* players = (Player *) ((MobileBlockThreadArg*) arg)->players;
    int* nPlayers = (int *) ((MobileBlockThreadArg*) arg)->nPlayers;

    Position pos;
    Position nextPos;

    srand48((unsigned int)time(NULL));

//    signal(SIGTERM, mobileBlockThreadSignalHandler);

    // Initialize position
    pthread_mutex_lock(&mutx->currentMap);
    while(true) {
        pos.y = rand() % (MAP_LINES + 1);
        pos.x = rand() % (MAP_COLS + 1);
        if( posIsValid(pos) && posIsFree(pos, currentMap) ) {
            break;
        }
    }
    currentMap->cmap[pos.y][pos.x] = CHAR_MBLOCKS;
    pthread_mutex_unlock(&mutx->currentMap);

    ModifyMapMessage initialMsg = {
            .pos = pos, .symbol = CHAR_MBLOCKS
    };
    broadcastMessageToPlayers(players, *nPlayers, ModifyMap, &initialMsg, sizeof(initialMsg), &mutx->players);
    printf("\nFoi inserido um novo bloqueio movel em y:%d x:%d.\n", pos.y, pos.x);
    fflush(stdout);

    while(true) {
        pthread_mutex_lock(&mutx->currentMap);
        do {
            nextPos = pos;
            // chose horizontal or vertical direction:
            if(drand48() < 0.5){
                // if horizontal
                if(drand48() < 0.5)
                    nextPos.x -= 1;    // left
                else
                    nextPos.x += 1;    // right
            }
            else{
                // if vertical
                if(drand48() < 0.5)
                    nextPos.y -= 1;    // up
                else
                    nextPos.y += 1;    // down
            }
        } while ( !posIsValid(nextPos) || !posIsFree(nextPos, currentMap) );

        currentMap->cmap[nextPos.y][nextPos.x] = CHAR_MBLOCKS;
        pthread_mutex_unlock(&mutx->currentMap);

        MoveMessage msg = {
                .from = pos, .to = nextPos, .symbol = CHAR_MBLOCKS
        };
        broadcastMessageToPlayers(players, *nPlayers, Move, &msg, sizeof(msg), &mutx->players);

        pos = nextPos;
        sleep(1);
    }
}