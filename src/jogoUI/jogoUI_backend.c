#include "jogoUI_backend.h"


void clearMap(Map* map);

void* communicationsThread(void* arg) {
    int myPipe = (int) ((CommunicationsThreadArg*) arg)->myPipe;
    Map * map = (Map*) ((CommunicationsThreadArg*) arg)->map;
    Windows * windows = (Windows*) ((CommunicationsThreadArg*) arg)->windows;
    bool *motorOrderedTerminate = (bool*) ((CommunicationsThreadArg*) arg)->motorOrderedTerminate;

    int nRocks = 0, nMBlocks = 0;
    bool running = true;

    while(running) {
        switch ( readNextMessageType(myPipe) ) {
            case NewLevel: {
                NewLevelMessage msg;
                if ( ! readNextMessage(myPipe, &msg, sizeof(msg)) ) {
                    executeCommand("Erro ao ler a proxima mensagem no pipe.", windows->notificationwin);
                    break;
                }
                copyMap(map, &msg.map);

                mostraMapa(windows->mapawin, *map);
                nivel(windows->nivelwin, msg.level);
                nRocks = 0; pedras(windows->pedraswin, nRocks);
                nMBlocks = 0; bloqueios(windows->bloqueioswin, nMBlocks);
                break;
            }
            case Move: {
                MoveMessage msg;
                if ( ! readNextMessage(myPipe, &msg, sizeof(msg)) ) {
                    executeCommand("Erro ao ler a proxima mensagem no pipe.", windows->notificationwin);
                    break;
                }

                map->cmap[msg.from.y][msg.from.x] = FREE_SPACE;
                map->cmap[msg.to.y][msg.to.x] = msg.symbol;
                mostraMapa(windows->mapawin, *map);
                break;
            }
            case ModifyMap: {
                ModifyMapMessage msg;
                if ( ! readNextMessage(myPipe, &msg, sizeof(msg)) ) {
                    executeCommand("Erro ao ler a proxima mensagem no pipe.", windows->notificationwin);
                    break;
                }
                // Check if its going to add or remove a rock or mobile block
                if( map->cmap[msg.pos.y][msg.pos.x] == CHAR_ROCK && msg.symbol == FREE_SPACE ){
                    nRocks--;   // if its going to clear a rock
                    pedras(windows->pedraswin, nRocks); // update rock counter
                    executeCommand("Foi removida uma pedra.", windows->notificationwin);
                }
                else if( map->cmap[msg.pos.y][msg.pos.x] == FREE_SPACE && msg.symbol == CHAR_ROCK ){
                    nRocks++;   // If its going to add a rock
                    pedras(windows->pedraswin, nRocks);
                    executeCommand("Foi adicionada uma pedra.", windows->notificationwin);
                }
                else if( map->cmap[msg.pos.y][msg.pos.x] == CHAR_MBLOCKS && msg.symbol == FREE_SPACE ){
                    nMBlocks--;   // if its going to clear a mobile block
                    bloqueios(windows->bloqueioswin, nMBlocks);
                    executeCommand("Foi removido um bloqueio movel.", windows->notificationwin);
                }
                else if ( map->cmap[msg.pos.y][msg.pos.x] == FREE_SPACE && msg.symbol == CHAR_MBLOCKS ){
                    nMBlocks++;
                    bloqueios(windows->bloqueioswin, nMBlocks);
                    executeCommand("Foi adicionado um bloqueio movel.", windows->notificationwin);
                }

                map->cmap[msg.pos.y][msg.pos.x] = msg.symbol;

                mostraMapa(windows->mapawin, *map);

                break;
            }
            case PlayersList: {
                PlayersListMessage msg;
                if ( ! readNextMessage(myPipe, &msg, sizeof(msg)) ) {
                    executeCommand("Erro ao ler a proxima mensagem no pipe.", windows->notificationwin);
                    break;
                }
                executeCommand("Erro ao ler a proxima mensagem no pipe.", windows->notificationwin);
                int stringSize = (msg.nPlayers*MAX_PLAYER_NAME) + (msg.nPlayers*2) + 1;
                                //  size of all usernames  +  commas and spaces  +  /0
                char commaSeparatedList[stringSize];
                sprintf(commaSeparatedList, "%s", msg.players[0]);
                for(int i = 1; i < msg.nPlayers; i++)
                    sprintf(commaSeparatedList, "%s, %s", commaSeparatedList, msg.players[i]);

                // Print list in window
                executeCommand(commaSeparatedList, windows->notificationwin);
                break;
            }
            case TextMsg: {
                TextMessage msg;
                if ( ! readNextMessage(myPipe, &msg, sizeof(msg)) ) {
                    executeCommand("Erro ao ler a proxima mensagem no pipe.", windows->notificationwin);
                    break;
                }
                char string[MAX_MESSAGE_SIZE*2];
                sprintf(string, "Nova mensagem de '%s': ", msg.from);
                strcat(string, msg.message);

                executeCommand(string, windows->notificationwin);
                break;
            }
            case Terminate: {
                clearMap(map);
                mostraMapa(windows->mapawin, *map);
                running = false;
                *motorOrderedTerminate = true;
                break;
            }
            default:
                executeCommand("Erro ao ler o tipo da proxima mensagem no pipe. Tipo irreconhecível.", windows->notificationwin);
        }
    }
}


void clearMap(Map* map) {
    for(int y = 0; y < MAP_LINES; y++)
        for(int x = 0; x < MAP_COLS; x++)
            map->cmap[y][x] = FREE_SPACE;
}