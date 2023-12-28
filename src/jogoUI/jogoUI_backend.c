#include "jogoUI_backend.h"



void* communicationsThread(void* arg) {
    int myPipe = (int) ((CommunicationsThreadArg*) arg)->myPipe;
    Map * map = (Map*) ((CommunicationsThreadArg*) arg)->map;
    Windows * windows = (Windows*) ((CommunicationsThreadArg*) arg)->windows;

    while(true) {
        switch ( readNextMessageType(myPipe) ) {
            case NewLevel: {
                NewLevelMessage msg;
                if ( ! readNextMessage(myPipe, &msg, sizeof(msg)) ) {
                    perror("\nErro ao ler a proxima mensagem no pipe.");
                    break;
                }
//                memcpy(map, &msg.map, sizeof(Map));
                copyMap(map, &msg.map);

                mostraMapa(windows->mapawin, 18, 81, NULL, *map);

                // todo show level

                break;
            }
            case ModifyMap: {
                ModifyMapMessage msg;
                if ( ! readNextMessage(myPipe, &msg, sizeof(msg)) ) {
                    perror("\nErro ao ler a proxima mensagem no pipe.");
                    break;
                }
                map->cmap[msg.pos.y][msg.pos.x] = msg.symbol;

                mostraMapa(windows->mapawin, 18, 81, NULL, *map);
                break;
            }
            default:
                perror("\nErro ao ler o tipo da proxima mensagem no pipe.");
        }
    }

    return NULL;
}