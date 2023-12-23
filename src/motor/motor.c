#include "motor.h"



void terminate(int exitcode){
    printf("\nTerminating...\n");

    // wait for all threads to join

    // Terminate all bots
    for( ;game->nBots > 0; game->nBots-- ){
        kill(game->bots[game->nBots-1].pid, SIGTERM);
//        wait(???); // esperar que os bots terminem
    }

    // Close general pipe
    if(game->generalPipe > 0)
        close(game->generalPipe);
    unlink(GENERAL_PIPE);

    // Close client pipes

    free(game);
    fflush(stdout);
    exit(exitcode);
}






bool isLinhaEmBranco(const char *linha) {
    while (*linha) {
        if (!isspace(*linha)) {
            return false;  // A linha não está em branco
        }
        linha++;
    }
    return true;  // A linha está em branco
}

int lerMapas() {
    char filepath[100], filename[20];

//    switch (game->currentLevel) {
//        case 1: snprintf(filepath, sizeof(filepath), "./maps/%s", "labN1.txt");
//            break;
//        case 2: snprintf(filepath, sizeof(filepath), "./maps/%s", "labN2.txt");
//            break;
//        case 3: snprintf(filepath, sizeof(filepath), "./maps/%s", "labN3.txt");
//            break;
//    }

    for(int i = 0; i < MAX_LEVELS; i++){

        snprintf(filename, sizeof(filename), "labN%d.txt", i+1);
        snprintf(filepath, sizeof(filepath), "./maps/%s", filename);

        FILE *filePointer;
        filePointer = fopen(filepath, "r");

        if (filePointer == NULL) {
            fprintf(stderr, "Erro ao abrir o ficheiro %s", filepath);
            return EXIT_FAILURE;
        }

        int linhaAtual = 0;
        char buffer[MAP_COLS + 2];  // +2 para incluir espaço para '\n' e '\0'

        while (linhaAtual < MAP_LINES && fgets(buffer, sizeof(buffer), filePointer) != NULL) {
            size_t comprimento = strlen(buffer);

            // Remover o caractere de nova linha, se existir
            if (comprimento > 0 && buffer[comprimento - 1] == '\n') {
                buffer[comprimento - 1] = '\0';
            }

            // Verificar se a linha não está em branco
            if (!isLinhaEmBranco(buffer)) {
                strncpy(map[i].cmap[linhaAtual], buffer, MAP_COLS);
                linhaAtual++;
            }
        }

        fclose(filePointer);
    }
    return EXIT_SUCCESS;
}





int testBot(Bot *bot) {
    if (pipe(bot->pipe_fd) == -1) {
        perror("Erro ao criar pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == 0) {
        // Processo do bot
        close(bot->pipe_fd[0]);  // Fecha a extremidade de leitura do pipe no processo do bot
        close(STDOUT_FILENO);    // Fecha STDOUT padrão

        // Substitui a imagem do processo do bot com o programa ./bot
        dup(bot->pipe_fd[1]);  // Duplica a extremidade de escrita do pipe para STDOUT
        close(bot->pipe_fd[1]);  // Fecha a extremidade de escrita do pipe (ainda é mantida por STDOUT)

        execlp("./bot", "./bot", "1", "10", (char *)NULL);

        // Se execlp() falhar
        perror("Erro ao executar o bot");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Processo pai
        close(bot->pipe_fd[1]);  // Fecha a extremidade de escrita do pipe no processo pai
        bot->pid = pid;
        return bot->pipe_fd[0];  // Retorna a extremidade de leitura do pipe no processo pai
    } else {
        perror("Erro ao criar processo para o bot");
        exit(EXIT_FAILURE);
    }
}

void runBots(Game *game) {
    if (game->nBots < MAX_BOTS) {
        Bot newBot;
        snprintf(newBot.name, sizeof(newBot.name), "Bot%d", game->nBots + 1);

        // Executa o bot e obtém a extremidade de leitura do pipe
        int pipe_fd = testBot(&newBot);

        // Armazena a informação do bot no jogo
        game->bots[game->nBots++] = newBot;

        printf("Bot lançado: %s\n", newBot.name);
        printf("Pipe FD para %s: %d\n", newBot.name, pipe_fd);
    } else {
        printf("Limite de bots alcançado\n");
    }
}

int processAdminCommand(char *adminCommand, GameSettings *gameSettings) {

    if (strcmp(adminCommand, "run_bots") == 0) {
        runBots(game);
    } else

    if (strcmp(adminCommand, "users") == 0) {
        printf("\nComando \"users\"\n");
    }

    else if (strncmp(adminCommand, "kick", 4) == 0) {

        char *token = strtok(adminCommand, " ");
        char *targetName = strtok(NULL, " ");

        if (targetName != NULL) {

            char *aux;
            aux = (char *)malloc(strlen(targetName) + strlen(" banido") + 1);
            strcpy(aux,targetName);
            strcat(aux," banido");
            printf("\n");
            puts(aux);
            printf("\n");
            free(aux);
        }
        else {
            printf("\nkick <nome do jogador>\n");
        }
    }

    else if (strcmp(adminCommand, "bots") == 0) {
        printf("\nComando \"bots\"\n");
    }

    else if (strcmp(adminCommand, "bmov") == 0) {
        printf("\nComando \"bmov\"\n");
    }

    else if (strcmp(adminCommand, "rbm") == 0) {
        printf("\nComando \"rbm\"\n");
    }

    else if (strcmp(adminCommand, "begin") == 0) {
        printf("\nComando \"begin\"\n");
    }

    else if (strcmp(adminCommand, "end") == 0) {
        return -1;
    }

    else {
        printf("Comando do administrador inválido: %s\n", adminCommand);
    }
    return 0;
}



void* gameThread(void* arg) {
//    Game* game = (Game*) arg;

    game->currentLevel = 1;
    while(game->currentLevel <= MAX_LEVELS ) {
        printf("Iniciou o nivel %d", game->currentLevel);
        fflush(stdout);

        NewLevelMessage newMap = {game->currentLevel};
        memcpy(&newMap, &map[game->currentLevel-1], sizeof(Map));

        for(int i = 0; i < game->nPlayers; i++)
            if( ! writeMessage(game->players[i].pipe, NewLevel, &newMap, sizeof(newMap)) ){
                fprintf(stderr, "Erro ao enviar mapa")
            }

        sleep(1000000000);
        game->currentLevel++;
    }
}


int main(int argc, char *argv[]) {
    GameSettings gameSettings;

    // Inicialize o gerador de números aleatórios com uma semente
    srand((unsigned int)time(NULL));

    // Read all maps
    if( lerMapas() == EXIT_FAILURE)
        return EXIT_FAILURE;

    // Initialize Game Structure
    game = malloc(sizeof(Game));
    if (game == NULL){
        perror("\nERRO: ao alocar memoria para o jogo.\n");
        return EXIT_FAILURE;
    }
    resetGame(game);


    // Read Environment Variables
    if( readEnvironmentVariables(&gameSettings) == EXIT_FAILURE )
        terminate(EXIT_FAILURE);


    // Register signal handler
    signal(SIGINT, terminate);
    signal(SIGTERM, terminate);


    // Create and open general pipe
    game->generalPipe = create_and_open(GENERAL_PIPE, O_RDWR);

    if (game->generalPipe == -1){
        perror("\nERRO: nao foi possivel abrir o mecanismo de comunicacao (pipe) geral.\n");
        terminate(EXIT_FAILURE);
    } else if (game->generalPipe == -2) {
        printf("\nJa existe uma instancia do Motor a executar! So e permitido uma!\n");
        terminate(EXIT_FAILURE);
    } else if (game->generalPipe == -3) {
        perror("\nERRO: falha ao criar o mecanismo de comunicacao (pipe) geral.\n");
        terminate(EXIT_FAILURE);
    }


    while(true){
        printf("\n\nA iniciar novo jogo!\n");

        if( waitForClientsSignUp(gameSettings, game) == EXIT_FAILURE) {
            terminate(EXIT_FAILURE);
        }

        // Inicialize as coordenadas x e y para todos os jogadores
//        initializePlayerCoordinates(game.players, game->nPlayers);

        pthread_t id;
        pthread_create(&id, NULL, gameThread, NULL );

        pthread_join(id, NULL);
    }


    char adminCommand[50];
    while (1) {

        printf("\nInsira um comando do administrador: ");
        fgets(adminCommand, sizeof(adminCommand), stdin);
        printf("\n");
        adminCommand[strcspn(adminCommand, "\n")] = '\0';  // Remova a nova linha

        // Processa o comando do administrador
        if( processAdminCommand(adminCommand, &gameSettings) == -1)
            break;
    }

    terminate(0);
    return 0;
}

