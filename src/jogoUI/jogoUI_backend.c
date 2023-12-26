#include "jogoUI_backend.h"



void* communicationsThread(void* arg) {
    int myPipe = ((CommunicationsThreadArg*) arg)->myPipe;
    Map * map = ((CommunicationsThreadArg*) arg)->map;

    while(true) {
        switch ( readNextMessageType(myPipe) ) {
            case NewLevel: {
                NewLevelMessage msg;
                if ( ! readNextMessage(myPipe, &msg, sizeof(msg)) ) {
                    perror("\nErro ao ler a proxima mensagem no pipe.");
                    break;
                }
                memcpy(map, &msg.map, sizeof(Map));

                // TODO - mostraMapa()

                break;
            }
            default:
                perror("\nErro ao ler o tipo da proxima mensagem no pipe.");
        }
    }

    return NULL;
}