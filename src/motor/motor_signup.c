#include "motor_signup.h"

void receiveNewPlayer(Game* game);
void printCommandInstructions();
int readSignupCommands(Game* game);
bool keepWaiting();


int waitForClientsSignUp(GameSettings gameSettings, Game* game){
    /*
     * Wait for enough players to sign up
     */
    bool hasTimedOut = false;
    game->nPlayers = 0;

    // To be used with select()
    struct timeval waitTime = {gameSettings.signupWindowDurationSeconds, 0};
    fd_set selectHandler;
    int pipesToWatch[2] = {game->generalPipe, 0};

    printf("\n\nEsta aberta a inscricao de jogadores...\n");

    while(game->nPlayers < MAX_PLAYERS &&
          !(hasTimedOut && game->nPlayers >= gameSettings.minPlayers)
        // sai se ja terminou a espera, mas entretanto ja tem o minimo de jogadores
            ){

        int sval = selectPipe(&selectHandler, pipesToWatch, sizeof(pipesToWatch), &waitTime);

        if( pipeIsSet(game->generalPipe, &selectHandler) ){
            // Read pipe
            receiveNewPlayer(game);
        }
        if( pipeIsSet(0, &selectHandler) ){
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


void receiveNewPlayer(Game* game){
    /*
     * read a new player from the general pipe
     */

    Player* newPlayer = &game->players[game->nPlayers];

    SignUpMessage msg;

    if( readNextMessageType(game->generalPipe) != SignUp ){
        // todo clean pipe?
        perror("\nErro ao ler o tipo da proxima mensagem no pipe.");
        return;
    }

    if( ! readNextMessage(game->generalPipe, &msg, sizeof(msg)) ){
        perror("\nErro ao ler a proxima mensagem no pipe.");
        return;
    }

    // If username already exists, means user restarted client app, so pipe and pid are different
    for(int i = 0; i < game->nPlayers; i++)
        if(strncmp(game->players[i].username, msg.username, MAX_PLAYER_NAME) == 0)
            newPlayer = &game->players[i];

    newPlayer->pipe = open(msg.pipePath, O_WRONLY);
    if (newPlayer->pipe == -1){ //se o pipe do cliente der erro a abrir!
        fprintf(stderr, "\nERRO ao abrir o pipe com o cliente %s\n", msg.username);
        kill(msg.pid, SIGTERM);
        return;
    }

    // Send confirmation message to player
    int success = SignUpSuccessful;
    if( write(newPlayer->pipe, &success, sizeof(int)) != sizeof(int) ){
        fprintf(stderr, "\nERRO ao enviar mensagem de sucesso ao cliente %s.\n", msg.username);
        kill(msg.pid, SIGTERM);
        return;
    }

    // Save user details
    strcpy(newPlayer->username, msg.username);
    newPlayer->pid = msg.pid;

    printf("\nO utilizador '%s' inscreveu-se no jogo.", newPlayer->username);

    game->nPlayers++;
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