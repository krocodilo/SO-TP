#include "motor_signup.h"

void receiveNewPlayer(Game* game);
void printCommandInstructions();
int readSignupCommands(Game* game);
bool keepWaiting();


int waitForClientsSignUp(GameSettings gameSettings, Game* game, Mutexes *mutx){
    /*
     * Wait for enough players to sign up
     */
    bool hasTimedOut = false;
    game->nPlayers = 0;

    // To be used with select()
    struct timeval waitTime = {gameSettings.signupWindowDurationSeconds, 0};
    fd_set selectHandler;
    int pipesToWatch[2] = {game->generalPipe, STDIN_FILENO};

    printf("\n\nEsta aberta a inscricao de jogadores...\n");

    printCommandInstructions();

    while(game->nPlayers < MAX_PLAYERS &&
          !(hasTimedOut && game->nPlayers >= gameSettings.minPlayers)
        // sai se ja terminou a espera, mas entretanto ja tem o minimo de jogadores
            ){
        printf("\n>  ");
        fflush(stdout);

        // Wait for something
        int sval = selectPipe(&selectHandler, pipesToWatch, 2, &waitTime);

        if( pipeIsSet(game->generalPipe, &selectHandler) ){
            // Read pipe
            switch (readNextMessageType(game->generalPipe)) {
                case SignUp: {
                    receiveNewPlayer(game);
                    break;
                }
                case LeaveGame: {
                    GenericRequestMessage msg;
                    if ( ! readNextMessage(game->generalPipe, &msg, sizeof(msg)) ) {
                        perror("\nErro ao ler a proxima mensagem no pipe.");
                        break;
                    }
                    Player *player = getPlayerByPID(msg.pid, game->players, game->nPlayers, &mutx->players);
                    if(player == NULL){
                        fprintf(stderr, "\nERRO - recebeu mensagem de um jogador não registado. PID: %d.", msg.pid);
                        break;
                    }
                    char name[MAX_PLAYER_NAME];
                    strcpy(name, player->username);
                    // Remove player
                    removePlayer(player, game->players, &game->nPlayers, &mutx->players);
                    printf("\nFoi eliminado o jogador %s.\n", name);
                    break;
                }
                default: perror("\nErro ao ler o tipo da proxima mensagem no pipe.");
            }
        }
//        if( FD_ISSET(STDIN_FILENO, &selectHandler) ){
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
                printf("Continuando a espera de novos jogadores, ate atingir o minimo.");
            }
            else
                return EXIT_FAILURE;
        }
        else if( sval == -1 )
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

//    if( readNextMessageType(game->generalPipe) != SignUp ){
//        perror("\nErro ao ler o tipo da proxima mensagem no pipe.");
//        return;
//    }

    if( ! readNextMessage(game->generalPipe, &msg, sizeof(msg)) ){
        perror("\nErro ao ler a proxima mensagem no pipe.");
        return;
    }

    // If username already exists, means user restarted client app, so pipe and pid are different
    for(int i = 0; i < game->nPlayers; i++)
        if(strncmp(game->players[i].username, msg.username, MAX_PLAYER_NAME) == 0){
            newPlayer = &game->players[i];
            game->nPlayers--;
        }

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
    newPlayer->symbol = (char) toupper(newPlayer->username[0]);

    printf("\nO utilizador '%s' inscreveu-se no jogo.\n", newPlayer->username);

    game->nPlayers++;
}

//imprimir lista de comandos
void printCommandInstructions() {
    printf("\nComandos disponíveis:\n");
    printf("\t- begin   : Iniciar o jogo\n");
    printf("\t- users   : Listar jogadores inscritos\n");
    printf("\t- end     : Sair do jogo\n");
}

//ler comandos
int readSignupCommands(Game* game) {
    char command[MAX_COMMAND_LENGTH];

    if (fgets(command, sizeof(command), stdin) == NULL) {
        perror("\nErro ao ler o comando.\n");
        return EXIT_FAILURE;
    }

    if (strncmp(command, "begin", 5) == 0) {
        // Verificar condições para iniciar o jogo
        if (game->nPlayers >= 1) {
            printf("\nIniciando o jogo manualmente...\n");
            return EXIT_SUCCESS;
        } else
            printf("\nNúmero insuficiente de jogadores para iniciar o jogo. É necessario pelo menos 1 jogador.\n");
    }
    else if (strncmp(command, "users", 5) == 0) {
        // Exibir informações dos jogadores
        if(game->nPlayers > 0)
            for (int i = 0; i < game->nPlayers; i++)
                printf("Jogador #%d: %s\n", i + 1, game->players[i].username);
        else
            printf("\nNão existem jogadores inscritos.\n");
        return -1;
    }
    else if (strncmp(command, "end", 3) == 0) {
        // Sair do jogo
        return EXIT_FAILURE;
    } else
        printCommandInstructions();

    return -1;
}


bool keepWaiting() {
    printf("\nO periodo de inscricoes terminou, sem o minimo de jogadores. Pretende "
           "esperar pelo minimo numero de jogadores? \n(yes/no, DEFAULT: yes) _>  ");

    char resp[5];
    fgets(resp, sizeof(resp), stdin);
    resp[strcspn(resp, "\n")] = '\0';

    if (strcmp(resp, "no") == 0)
        return false;

    return true;
}