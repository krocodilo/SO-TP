#include "utils.h"



int create_and_open(char* pipename, int flags) {
    /*
    Recebe:
        -> string do nome do pipe a criar com permissoes 0666
        -> flags a passar a funcao open
    Devolve:
        file deescriptor do pipe em caso de sucesso
        -1  ->  caso nao consiga abrir o pipe criado
        -2  ->  caso ja exista um pipe com esse nome
        -3  ->  caso ocorra erro ao criar o pipe
    */
    //Cria o pipe generico
    if (mkfifo(pipename, 0666) == 0){
        //do nothing
    }else if (errno == EEXIST){
        // Ja existe um pipe com esse nome
        return -2;
    }else{
        // falha ao criar o pipe.
        return -3;
    }

    int pipe_fd = open(pipename, flags);
    if(pipe_fd == -1){
        // nao foi possivel abrir o pipe
        unlink(pipename);   //elimina o pipe
    }
    return pipe_fd;
}


int readNextMessageType(int pipe_fd) {
    // Returns -1 if error occured
    int msgType;
    if ( read(pipe_fd, &msgType, sizeof(int)) != sizeof(int) )
        return -1;
    return msgType;
}


bool readNextMessage(int pipe_fd, void* buffer, int size) {

    if ( read(pipe_fd, buffer, size) != size )
        return false;
    return true;
}


bool writeMessage(int pipe_fd, int msgType, void* message, int size) {

    if( write(pipe_fd, &msgType, sizeof(int)) != sizeof(int) )
        return false;
    if( write(pipe_fd, message, size) != size )
        return false;
    return true;
}


int selectPipe(fd_set * selectHandler, int pipes[], int numPipes, struct timeval * waitTime) {
    /*
     * Wrapper function for select()
     */

    int highestPipeFD = 0;

    FD_ZERO(selectHandler);

    for(int i = 0; i < numPipes; i++) {
        FD_SET(pipes[i], selectHandler);
        if(pipes[i] > highestPipeFD)
            highestPipeFD = pipes[i];
    }

    return select(
            highestPipeFD + 1,  // este fd é o maior de todos os fds
            selectHandler, NULL, NULL,
            waitTime               // cada uso do select decrementa o waitTime. Pode ser NULL
    );
}


int pipeIsSet(int pipeFd, fd_set * selectHandler) {
    return FD_ISSET(pipeFd, selectHandler);
}


void copyMap(Map * to, Map * from) {
    for(int l = 0; l < MAP_LINES; l++)
        for(int c = 0; c < MAP_COLS; c++)
            to->cmap[l][c] = from->cmap[l][c];
}


void flushPipe(int pipe_fd) {
    char buffer[BUFSIZE];
    while( read(pipe_fd, buffer, BUFSIZE) > 0 ){

    }
}