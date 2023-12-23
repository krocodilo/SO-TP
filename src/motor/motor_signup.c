#include "motor_signup.h"

bool receiveNewPlayer(Player* newPlayer, int generalPipe);
void printCommandInstructions();
int readSignupCommands(Game* game);
bool keepWaiting();


int waitForClientsSignUp(GameSettings gameSettings, Game* game){
    /*
     * Wait for enough players to sign up
     */

    //cosntrucao da struct timeval para usar no select
    struct timeval waitTime = {gameSettings.signupWindowDurationSeconds, 0};
    fd_set read_fds;
    int sval, hasTimedOut = false;

    printf("\n\nEsta aberta a inscricao de jogadores...\n\n");

    while(game->nPlayers < MAX_PLAYERS &&
          !(hasTimedOut && game->nPlayers >= gameSettings.minPlayers)
        // sai se ja terminou a espera, mas entretanto ja tem o minimo de jogadores
            ){

        FD_ZERO(&read_fds);         //inicializa a watchlist
        FD_SET(game->generalPipe, &read_fds);   // add generalPipe ao conjunto watchlist
        FD_SET(0, &read_fds);                   // STDIN

        sval = select(
                game->generalPipe + 1,  // este fd é o maior de todos os fds
                &read_fds, NULL, NULL,
                &waitTime               // cada uso do select decrementa o waitTime
        );

        if (FD_ISSET(game->generalPipe, &read_fds)) {
            // Read pipe
            if ( receiveNewPlayer(&game->players[game->nPlayers], game->generalPipe) )
                game->nPlayers++;
        }
        if (FD_ISSET(0, &read_fds)) {
            // Read STDIN
            switch ( readSignupCommands(game) ) {
                case EXIT_FAILURE: return EXIT_FAILURE;
                case EXIT_SUCCESS: return EXIT_SUCCESS;
            }
        }
        else if(sval == 0) {  // Timeout
            if(game->nPlayers >= gameSettings.minPlayers) {
                printf("\n\nO periodo de inscricoes terminou, existindo o minimo de jogadores.");
                break;
            }
            if( keepWaiting() ){
                waitTime.tv_sec = -1;   // remove timeout from select
                hasTimedOut = true;
                printf("Em espera por novos jogadores, ate atingir o minimo.");
            }
            else
                break;
        }
        else if(sval == -1)
            perror("\n\nErro na espera de jogadores.");

    }
    return EXIT_SUCCESS;
}


bool receiveNewPlayer(Player* newPlayer, int generalPipe){
    /*
     * read a new player from the general pipe
     */

    SignUpMessage msg;

    if( readNextMessageType(generalPipe) != SignUp ){
        // todo clean pipe?
        perror("\nErro ao ler o tipo da proxima mensagem no pipe.");
        return false;
    }

    if( ! readNextMessage(generalPipe, &msg, sizeof(msg)) ){
        perror("\nErro ao ler a proxima mensagem no pipe.");
    }

    // Todo - if username already exists, means the pipe is new, so replace it here





    strcpy(newPlayer->username, msg.username);

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


void printCommandInstructions() {
    // start, players, exit
}


int readSignupCommands(Game* game) {
    // todo
    // start -> existe numero minimo de players (2)?
    printf("\nreading cmd\n");
    return EXIT_SUCCESS;
}


bool keepWaiting() {
    printf("\nO periodo de inscricoes terminou, sem o minimo de jogadores. Pretende "
           "esperar pelo minimo numero de jogadores? \n(yes/no, DEFAULT: yes) _>  ");

    char resp[5];
    fgets(resp, sizeof(resp), stdin);
    resp[strcspn(resp, "\n")] = '\0';

    if (strcmp(resp, "no") == 0)
        return false;

    printf("Continuando a espera...");
    return true;
}