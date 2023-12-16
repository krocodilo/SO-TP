#include "motor_signup.h"




void waitForClientsSignUp(GameSettings gameSettings, Game* game){
    /*
     * Wait for enough players to sign up
     */

    //cosntrucao da struct timeval para usar no select
    struct timeval waitTime = {gameSettings.signupWindowDurationSeconds, 0};
    fd_set read_fds;
    int sval, hasTimedOut = 0;

    printf("\n\nEsta aberta a inscricao de jogadores...\n\n");

    while(game->nPlayers < MAX_PLAYERS &&
          !(hasTimedOut && game->nPlayers >= gameSettings.minPlayers)
        // sai se ja terminou a espera, mas entretanto ja tem o minimo de jogadores
            ){

        if(!hasTimedOut){
            FD_ZERO(&read_fds);            //inicializa a watchlist
            FD_SET(game->generalPipe, &read_fds); // add generalPipe ao conjunto watchlist

            sval = select(
                    game->generalPipe + 1,
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
        if( receiveNewPlayer( &game->players[game->nPlayers], game->generalPipe) )
            game->nPlayers++;
    }
}


bool receiveNewPlayer(Player* newPlayer, int generalPipe){
    /*
     * read a new player from the general pipe
     */

    SignUpMessage msg;

    if (read(generalPipe, &msg, sizeof(msg)) != sizeof(msg) ){
        perror("\nERRO: foi recebida uma mensagem incompleta aquando tentativa de inscricao.\n");
        return false;
    }

    strcpy(newPlayer->username, msg.username);

    // Concatenate directory and filename
//    char clientPipeName[PIPE_PATH_MAX_SIZE];
//    strcpy(clientPipeName, PIPE_DIRECTORY);
//    strcat(clientPipeName, newPlayer->username);

    newPlayer->pipe = open(msg.pipePath, O_RDONLY);

    if (newPlayer->pipe == -1){ //se o pipe do cliente der erro a abrir!
        fprintf(stderr, "\nERRO ao abrir o pipe com o cliente %s\n", newPlayer->username);
        return false;
        // opcionalmente, enviar um sinal ao cliente para informar da impossibilidade
    }

    // TODO send confirmation message to player

    printf("\nO utilizador '%s' inscreveu-se no jogo.\n", newPlayer->username);
    return true;
}