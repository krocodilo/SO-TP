#include "motor_bots.h"



void* botThread(void * arg) {
    // Parse arguments into local pointers
//    Bot *bot = (Bot *) ((BotThreadArg *) arg)->thisBot;
    int botId = (int) ((BotThreadArg *) arg)->botId;
    int botPipe = (int) ((BotThreadArg *) arg)->botPipe;
    int duration = (int) ((BotThreadArg *) arg)->duration;
    Map *currentMap = (Map *) ((BotThreadArg *) arg)->currentMap;
    Mutexes *mutx = (Mutexes *) ((BotThreadArg *) arg)->mutexes;
    Player *players = (Player *) ((BotThreadArg *) arg)->players;
    int nPlayers = (int) ((BotThreadArg *) arg)->nPlayers;

    char output[100];

    while(true){
        ModifyMapMessage msg = {.symbol = CHAR_ROCK};

        // Wait for new coordinates
        read(botPipe, output, sizeof(output));
        if( sscanf(output, "%d %d ", &msg.pos.x, &msg.pos.y) != 2 ){
            // If it can't read two integers
            fprintf(stderr, "\nERRO ao ler ouput do bot #%d: %s", botId, output);
            break;
        }

        // chech if position is free
        pthread_mutex_lock(&mutx->currentMap);
        if( currentMap->cmap[msg.pos.y][msg.pos.x] != FREE_SPACE ){
            pthread_mutex_unlock(&mutx->currentMap);
//            printf("\nBot #%d queria meter num espaco ocupado - y:%d x:%d", bot->id, msg.pos.y, msg.pos.x);
            fflush(stdout);
            continue;
        }
        currentMap->cmap[msg.pos.y][msg.pos.x] = CHAR_ROCK;   // assign position to this rock
        pthread_mutex_unlock(&mutx->currentMap);

        // Send rock to all players
        broadcastMessageToPlayers(players, nPlayers, ModifyMap, &msg, sizeof(msg), &mutx->players);
//        printf("\n- sent new rock %d %d-", msg.pos.y, msg.pos.x);
//        fflush(stdout);

        // Wait to remove the rock
        sleep(duration);

        pthread_mutex_lock(&mutx->currentMap);
        currentMap->cmap[msg.pos.y][msg.pos.x] = FREE_SPACE;   // free up position where rock is
        pthread_mutex_unlock(&mutx->currentMap);

        // Remove rock for all players
        msg.symbol = FREE_SPACE;
        broadcastMessageToPlayers(players, nPlayers, ModifyMap, &msg, sizeof(msg), &mutx->players);
//        printf("- removed rock %d %d-", msg.pos.y, msg.pos.x);
//        fflush(stdout);
    }
    return NULL;
}


int executeBot(Bot *bot, int period, int duration) {
    int botPipe[2];
    if (pipe(botPipe) == -1) {
        perror("Erro ao criar pipe para bot.\n");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();

    if (pid == 0) {
        // Processo do bot
        close(botPipe[0]);  // Fecha a extremidade de leitura do pipe no processo do bot
        close(STDOUT_FILENO);    // Fecha STDOUT default

        dup(botPipe[1]);  // Duplica a extremidade de escrita do pipe para STDOUT (posiçao mais baixa disponivel)
        close(botPipe[1]);  // Fecha a extremidade de escrita do pipe

        char periodArg[sizeof(int)];
        char durationArg[sizeof(int)];
        sprintf(periodArg, "%d", period);   // arguments have to be sent as strings
        sprintf(durationArg, "%d", duration);

        execlp("./bot", "./bot", periodArg, durationArg, (char *)NULL);

        perror("Erro ao executar o bot");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Processo pai
        close(botPipe[1]);  // Fecha a extremidade de escrita do pipe no processo pai
        bot->pipe = botPipe[0];
        bot->pid = pid;
        return EXIT_SUCCESS;  // Retorna a extremidade de leitura do pipe no processo pai
    }
    perror("Erro ao criar processo para o bot.\n");
    return EXIT_FAILURE;
}


void runBots(Game *game, Map *currentMap, Mutexes *mutexes) {
    // Execute bots according to game level

    int period = 10, duration =8;    // todo
//    int period = 30, duration = 5 + (game->currentLevel * 5);

    pthread_mutex_lock(&mutexes->bots);
    game->nBots = 0;
    while (game->nBots < game->currentLevel + 1) {

        if( game->nBots != 0 )
            sleep(1);   // necessary to avoid bots generating same values

        Bot * currentBot = &game->bots[game->nBots];

        if( executeBot(currentBot, period, duration) == EXIT_FAILURE )
            continue;

        currentBot->id = game->nBots;    // Bot ID = index in array
        currentBot->period = period;
        currentBot->duration = duration;

        // start thread
        BotThreadArg * arg = malloc(sizeof(BotThreadArg));
        if (game == NULL){
            perror("\nERRO: ao alocar memoria para um bot.\n");
            return;
        }
//        arg->thisBot = currentBot;
        arg->botId = currentBot->id;
        arg->botPipe = currentBot->pipe;
        arg->duration = currentBot->duration;
        arg->currentMap = currentMap;
        arg->mutexes = mutexes;
        arg->players = game->players;
        arg->nPlayers = game->nPlayers;
        if ( pthread_create(&currentBot->threadId, NULL, botThread, arg ) != 0){
            perror("\nERRO ao tentar criar thread para um bot");
            kill(currentBot->pid, SIGINT);
            continue;
        }

//        period -= 5;  // todo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//        duration -= 5;
        game->nBots++;
        printf("Bot #%d foi lançado.\n", currentBot->id);
    }
    pthread_mutex_unlock(&mutexes->bots);
}