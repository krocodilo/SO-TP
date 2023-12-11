#include "motor_signup.h"




void waitForClientsSignUp(GameSettings gameSettings){
    /*
     * Wait for enough players to sign up
     */

    //cosntrucao da struct timeval para usar no select
    struct timeval waitTime = {gameSettings.signupWindowDurationSeconds, 0};
    fd_set read_fds;
    int sval, hasTimedOut = 0;

    printf("\n\nA espera de jogadores...\n\n");

    while(game->nPlayers < MAX_PLAYERS &&
          !(hasTimedOut && game->nPlayers >= gameSettings.minPlayers)
        // sai se ja terminou a espera, mas entretanto ja tem o minimo de jogadores
            ){

        if(!hasTimedOut){
            FD_ZERO(&read_fds);            //inicializa a watchlist
            FD_SET(game->pipegen_fd, &read_fds); // add pipegen_fd ao conjunto watchlist

            sval = select(
                    game->pipegen_fd +1,
                    &read_fds, NULL, NULL,
                    &waitTime               // cada uso do select decrementa o waitTime
            );
            if(sval == 0){  // Timeout
                if(game->nPlayers >= gameSettings.minPlayers){
                    printf("\n\nO periodo de inscricoes terminou, existindo o minimo de jogadores.");
                    break;
                }
                // else:
                printf("\nO periodo de inscricoes terminou, sem o minimo de jogadores. Pretende "
                       "esperar pelo minimo numero de jogadores? \n(yes/no, DEFAULT: yes) _>  ");

                char resp[5];
                fgets(resp, sizeof(resp), stdin);
                resp[strcspn(resp, "\n")] = '\0';

                if (strcmp(resp, "no") == 0)
                    break;
                else
                    printf("Continuando a espera...");
                hasTimedOut = 1;
            }
            else if(sval == -1)
                perror("\n\nErro na espera de jogadores.");
        }


        // read pipe
        if( receiveNewPlayer( &game->players[game->nPlayers] ) )
            game->nPlayers++;
    }
}



bool receiveNewPlayer(Player* newPlayer){
    /*
     * read a new player from the general pipe
     */

    char username[MAX_PLAYER_NAME];

    if ( read(game->pipegen_fd, &username, sizeof(username)) != sizeof(username) ){
        perror("\nERRO: foi recebida uma mensagem incompleta aquando tentativa de inscricao.\n");
        return false;
    }

    strcpy(newPlayer->username, username);

    // Concatenate directory and filename
    char clientPipeName[MAX_PLAYER_NAME + strlen(PIPE_DIRECTORY)];
    strcpy(clientPipeName, PIPE_DIRECTORY);
    strcat(clientPipeName, newPlayer->username);

    newPlayer->pipe_fd = open(clientPipeName, O_RDWR);

    if (newPlayer->pipe_fd == -1){ //se o pipe do cliente der erro a abrir!
        fprintf(stderr, "\nERRO ao abrir o pipe com o cliente %s\n", newPlayer->username);
        return false;
        // opcionalmente, enviar um sinal ao cliente para informar da impossibilidade
    }


    //delete
    close(newPlayer->pipe_fd);

    // Informar o cliente do sucesso
//    sm.wait = 1;
//    strcpy(sm.message, "\nA espera de mais jogadores...\n");
//    write(p->pipe_fd, &sm, sizeof(sm));  //envia para os jogadores
//
//
//    printf("Foi adicionado o jogador: %s\n", msg.message);
//
//    close(p->pipe_fd);
//    p->pipe_fd = -1;    //-1 para indicar a funcao terminate que o pipe nao esta aberto
//    p->next = NULL;
//
//    p->continuaPlayer = 1
//    p->isOut = 0; //0 -> esta dentro do campeonato


    printf("\nO utilizador '%s' inscreveu-se no jogo.\n", newPlayer->username);
    return true;
}