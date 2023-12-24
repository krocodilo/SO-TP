#include "motor_bots.h"



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