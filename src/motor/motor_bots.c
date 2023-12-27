#include "motor_bots.h"



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

        // Substitui a imagem do processo do bot com o programa ./bot
        dup(botPipe[1]);  // Duplica a extremidade de escrita do pipe para STDOUT (posiçao mais baixa disponivel)
        close(botPipe[1]);  // Fecha a extremidade de escrita do pipe

        execlp("./bot", "./bot", period, duration, (char *)NULL);

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


void runBots(Game *game) {

    int period = 30, duration = 5 + (game->currentLevel * 5);

    game->nBots = 0;
    while (game->nBots < game->currentLevel + 1) {

        Bot * currentBot = &game->bots[game->nBots];

        if( executeBot(currentBot, period, duration) == EXIT_FAILURE )
            continue;

        // start thread

        currentBot->id = game->nBots;    // Bot ID = index in array
        currentBot->period = period;
        currentBot->duration = duration;

        period -= 5;
        duration -= 5;
        game->nBots++;
    }
}



void botThread(Bot * bot, Map * currentMap, Game * game, Mutexes * mutx) {

    ModifyMapMessage msg = {.symbol = CHAR_ROCK};

    struct timeval waitTime = {-1, 0};      // infinite timeout
    fd_set selectHandler;
    int pipesToWatch[1] = {bot->pipe};
    char output[100];

    while(true){

        // Wait for new coordinates
        int sval = selectPipe(&selectHandler, pipesToWatch, sizeof(pipesToWatch), &waitTime);
        if( sval == 0 ){
            // Timeout = current rock has to be removed

            // Free position in map
            pthread_mutex_lock(&mutx->currentMap);
            currentMap->cmap[msg.pos.x][msg.pos.y] = FREE_SPACE;   // assign position to this rock
            pthread_mutex_unlock(&mutx->currentMap);

            msg.symbol = FREE_SPACE;
        }
        else if( pipeIsSet(bot->pipe, &selectHandler) ){
            // If bot pipe has something to read:

            // read bot output from pipe
            read(bot->pipe, output, sizeof(output));
            if( sscanf(output, "%d %d ", &msg.pos.x, &msg.pos.y) != 2 ){
                // If it can't read two integers
                fprintf(stderr, "\nERRO ao ler ouput do bot #%d: %s", bot->id, output);
                break;
            }

            // chech if position is free
            pthread_mutex_lock(&mutx->currentMap);
            if( currentMap->cmap[msg.pos.x][msg.pos.y] != FREE_SPACE )
                continue;
            currentMap->cmap[msg.pos.x][msg.pos.y] = CHAR_ROCK;   // assign position to this rock
            pthread_mutex_unlock(&mutx->currentMap);

            waitTime.tv_sec = bot->duration;    // Select will wait until rock has to be removed
        }
        else {
            fprintf(stderr, "\nERRO durante a espera do bot #%d.\n", bot->id);
            break;
        }

        // broadcast changes
        broadcastMessageToPlayers(game, ModifyMap, &msg, sizeof(msg), &mutx->players);

    }

}