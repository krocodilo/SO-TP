#include "motor.h"



void terminate(int signum){
//    if (signum == SIGINT){
//        printf("\nReceived SIGINT.");
//    }
    printf("\nTerminating...\n");

    // Terminate all bots
    for( ;game->nBots > 0; game->nBots-- ){
        kill(game->bots[game->nBots-1].pid, SIGTERM);
//        wait(???); // esperar que os bots terminem
    }

    // Close general pipe
    if(game->pipegen_fd > 0)
        close(game->pipegen_fd);
    unlink(GENERAL_PIPE);

    // Close client pipes

    exit(exitcode);
}

//Função para verificar se há colisão na próxima posição
bool verificaColisao(char mapa[][MAP_COLS + 1], int nextY, int nextX) {
    // Ajusta as coordenadas do jogador para corresponder ao mapeamento no mapa
    int playerMapY = nextY - 1;
    int playerMapX = nextX / 2;

    // Verifica se a próxima posição contém 'X' (obstáculo)
    return mapa[playerMapY][playerMapX] == 'X';
}

int isLinhaEmBranco(const char *linha) {
    while (*linha) {
        if (!isspace(*linha)) {
            return 0;  // A linha não está em branco
        }
        linha++;
    }
    return 1;  // A linha está em branco
}

void lerMapa(Game  *game) {
    char filepath[100];  // Ajuste o tamanho conforme necessário

    if (game->currentLevel== 1){
        snprintf(filepath, sizeof(filepath), "./maps/%s", "labN1.txt");
        //snprintf(filepath, sizeof(filepath), "../src/jogoUI/%s", "labirinto.txt");
    }
    if (game->currentLevel == 2){
        snprintf(filepath, sizeof(filepath), "./maps/%s", "labN2.txt");
    }
    if (game->currentLevel == 3){
        snprintf(filepath, sizeof(filepath), "./maps/%s", "labN3.txt");
    }

    FILE *filePointer;
    filePointer = fopen(filepath, "r");

    if (filePointer == NULL) {
        perror("Erro ao abrir o arquivo");
        return;
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
            strcpy(map.cmap[linhaAtual], buffer);
            linhaAtual++;
        }
    }

    fclose(filePointer);
}



void runBots(Game *game) {

    if (game->nBots < MAX_BOTS) {
        Bot newBot;
        snprintf(newBot.name, sizeof(newBot.name), "Bot%d", game->nBots + 1);

        pid_t pid = fork();

        if (pid == 0) {
            // Processo do bot
            execlp("./bot", "./bot", "1", "10", (char *)NULL);
            // Se execlp() falhar
            perror("Erro ao executar o bot");
            exit(1);
        } else if (pid > 0) {
            // Processo pai
            newBot.pid = pid;
            game->bots[game->nBots++] = newBot;
            printf("Bot lançado: %s\n", newBot.name);
        } else {
            perror("Erro ao criar processo para o bot");
        }
    } else {
        printf("Limite de bots alcançado\n");
    }
}

void testBot(Game *game) {




    // TODO: Esta função já não será necessária para a meta 2, mas tem codigo que tem de ser aplicado à runBots()




//    runBots(game);
    
    if (game->nBots == 0) {
        // Seleciona o primeiro bot para teste (índice 0)
//        Bot *bot = &game->bots[0];

        // Cria um pipe para comunicação entre o motor e o bot
        int pipe_fd[2];
        if (pipe(pipe_fd) == -1) {
            perror("Erro ao criar o pipe");
            return;
        }

        pid_t pid = fork();

        if (pid == 0) {
            // Processo do bot
            close(pipe_fd[0]);  // Fecha a extremidade de leitura do pipe no processo do bot
            dup2(pipe_fd[1], STDOUT_FILENO);  // Redireciona a saída padrão para o pipe
            close(pipe_fd[1]);  // Fecha a extremidade de escrita do pipe no processo do bot

            // Executa o bot
            execlp("./bot", "./bot", "1", "10", (char *)NULL);

            // Se execlp() falhar
            perror("Erro ao executar o bot");
            exit(1);
        } else if (pid > 0) {
            // Processo pai
            close(pipe_fd[1]);  // Fecha a extremidade de escrita do pipe no processo pai

            char buffer[50];
            ssize_t bytesRead;

            // Lê as mensagens do bot a partir do pipe
            while ((bytesRead = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) {
                printf("RECEBI: %.*s", (int)bytesRead, buffer);
            }

            close(pipe_fd[0]);  // Fecha a extremidade de leitura do pipe no processo pai
        } else {
            perror("Erro ao criar processo para o bot");
        }
    } else {
        printf("Nenhum bot disponível para teste\n");
    }

}


int processAdminCommand(char *adminCommand, GameSettings *gameSettings) {

    if (strcmp(adminCommand, "run_bots") == 0) {
        runBots(game);
    } else if (strcmp(adminCommand, "read_map") == 0) {
        mostra_mapa("labirinto.txt");
    } else if (strcmp(adminCommand, "test_bot") == 0) {
        testBot(game);

    }


    else if (strcmp(adminCommand, "users") == 0) {
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






int main(int argc, char *argv[]) {
    GameSettings gameSettings;


    game = malloc(sizeof(Game));
    if (game == NULL){
        perror("\nERRO: ao alocar memoria para o jogo.\n");
        return EXIT_FAILURE;
    }
    resetGame(game);

    // Read Environment Variables
    if( readEnvironmentVariables(&gameSettings) == EXIT_FAILURE ){
        exitcode = EXIT_FAILURE;
        terminate(0);
    }

    // Register signal handler
    signal(SIGINT, terminate);
    signal(SIGTERM, terminate);

    // Create and open general pipe
    game->pipegen_fd = create_and_open(GENERAL_PIPE, O_RDWR);

    if (game->pipegen_fd == -1){
        perror("\nERRO: nao foi possivel abrir o mecanismo de comunicacao (pipe) geral.\n");
        exitcode = EXIT_FAILURE; terminate(0);
    } else if (game->pipegen_fd == -2) {
        printf("\nJa existe uma instancia do Motor a executar! So e permitido uma!\n");
        exitcode = EXIT_FAILURE; terminate(0);
    } else if (game->pipegen_fd == -3) {
        perror("\nERRO: falha ao criar o mecanismo de comunicacao (pipe) geral.\n");
        exitcode = EXIT_FAILURE; terminate(0);
    }

    while(true){
        printf("\n\nStarting a new game!\n");

        waitForClientsSignUp(gameSettings);
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

