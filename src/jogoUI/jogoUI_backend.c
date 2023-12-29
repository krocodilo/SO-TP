#include "jogoUI_backend.h"



void* communicationsThread(void* arg) {
    int myPipe = (int) ((CommunicationsThreadArg*) arg)->myPipe;
    Map * map = (Map*) ((CommunicationsThreadArg*) arg)->map;
    Windows * windows = (Windows*) ((CommunicationsThreadArg*) arg)->windows;

    int nRocks = 0, nMBlocks = 0;

    while(true) {
        switch ( readNextMessageType(myPipe) ) {
            case NewLevel: {
                NewLevelMessage msg;
                if ( ! readNextMessage(myPipe, &msg, sizeof(msg)) ) {
                    perror("\nErro ao ler a proxima mensagem no pipe.");
                    break;
                }
                copyMap(map, &msg.map);

                mostraMapa(windows->mapawin, 18, 81, NULL, *map);
                nivel(windows->nivelwin, msg.level);
                break;
            }
            case ModifyMap: {
                ModifyMapMessage msg;
                if ( ! readNextMessage(myPipe, &msg, sizeof(msg)) ) {
                    perror("\nErro ao ler a proxima mensagem no pipe.");
                    break;
                }
                // Check if its going to add or remove a rock or mobile block
                if( map->cmap[msg.pos.y][msg.pos.x] == CHAR_ROCK && msg.symbol == FREE_SPACE ){
                    nRocks--;   // if its going to clear a rock
                    pedras(windows->pedraswin, nRocks); // update rock counter
                }
                else if( map->cmap[msg.pos.y][msg.pos.x] == FREE_SPACE && msg.symbol == CHAR_ROCK ){
                    nRocks++;   // If its going to add a rock
                    pedras(windows->pedraswin, nRocks);
                }
                else if( map->cmap[msg.pos.y][msg.pos.x] == CHAR_MBLOCKS && msg.symbol == FREE_SPACE ){
                    nMBlocks--;   // if its going to clear a mobile block
                    bloqueios(windows->bloqueioswin, nMBlocks);
                }
                else if ( map->cmap[msg.pos.y][msg.pos.x] == FREE_SPACE && msg.symbol == CHAR_MBLOCKS ){
                    nMBlocks++;
                    bloqueios(windows->bloqueioswin, nMBlocks);
                }

                map->cmap[msg.pos.y][msg.pos.x] = msg.symbol;

                mostraMapa(windows->mapawin, 18, 81, NULL, *map);

                break;
            }
            case Move: {
                MoveMessage msg;
                if ( ! readNextMessage(myPipe, &msg, sizeof(msg)) ) {
                    perror("\nErro ao ler a proxima mensagem no pipe.");
                    break;
                }
                // Check if it's a mobile block
                if( map->cmap[msg.from.y][msg.from.x] == CHAR_MBLOCKS ){
                    nMBlocks--;     // if its going to clear a mobile block
                    bloqueios(windows->bloqueioswin, nMBlocks);
                }
                else if (msg.symbol == CHAR_MBLOCKS ){
                    nMBlocks++;     // if its going to add a mobile block
                    bloqueios(windows->bloqueioswin, nMBlocks);
                }

                map->cmap[msg.from.y][msg.from.x] = FREE_SPACE;
                map->cmap[msg.to.y][msg.to.x] = msg.symbol;
                mostraMapa(windows->mapawin, 18, 81, NULL, *map);
                break;
            }
            case PlayersList: {
                PlayersListMessage msg;
                if ( ! readNextMessage(myPipe, &msg, sizeof(msg)) ) {
                    perror("\nErro ao ler a proxima mensagem no pipe.");
                    break;
                }
                int stringSize = (msg.nPlayers*MAX_PLAYER_NAME) + (msg.nPlayers*2) + 1;
                                //  size of all usernames  +  commas and spaces  +  /0
                char commaSeparatedList[stringSize];
                sprintf(commaSeparatedList, "%s", msg.players[0]);
                for(int i = 1; i < msg.nPlayers; i++)
                    sprintf(commaSeparatedList, "%s, %s", commaSeparatedList, msg.players[i]);

                // Print list in window
                jogadores(windows->jogadoreswin, commaSeparatedList);
                break;
            }
            default:
                perror("\nErro ao ler o tipo da proxima mensagem no pipe. Tipo irreconhecível.");
        }
    }

    return NULL;
}