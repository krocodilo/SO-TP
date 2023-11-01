#include "motor.h"



void terminate(int signum){
    if (signum == SIGINT){
        exit(exitcode);
    }
//    else
}



int main(){

    int pipegen_fd = create_and_open(GENERAL_PIPE, 077);;

    // Pick one

    // Create and open general pipe
    if (mkfifo(GENERAL_PIPE, 0777) == 0){
        //do nothing
    }else if (errno == EEXIST){
        printf("Ja existe uma instancia Arbitro a executar! So e permitido uma!");
        exitcode = EXIT_FAILURE; terminate(0);
    }else{
        perror("\nERRO: falha ao criar o processo de comunicacao com os clientes");
        exitcode = EXIT_FAILURE; terminate(0);
    }

    pipegen_fd = open(GENERAL_PIPE, O_RDWR);
    if (pipegen_fd == -1){
        fprintf(stderr, "\nERRO: nao foi possivel abrir o mecanismo de comunicacao geral com os clientes.\n");
        exitcode = EXIT_FAILURE; terminate(0);
    }

    return 0;
}