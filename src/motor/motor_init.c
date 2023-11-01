#include "motor_init.h"


int create_and_open(char* pipename, int flags)
{
    /*
    Recebe:
        -> string do nome do pipe a criar com permissoes 0777
        -> flags a passar a funcao open
    Devolve:
        file deescriptor do pipe em caso de sucesso
        -1  ->  caso nao consiga abrir o pipe criado
        -2  ->  caso ja exista um pipe com esse nome
        -3  ->  caso ocorra erro ao criar o pipe
    */
    //Cria o pipe generico
    if (mkfifo(pipename, 0777) == 0){
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