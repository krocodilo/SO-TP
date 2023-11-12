#include "motor.h"



void terminate(int signum){
    if (signum == SIGINT){
        exit(exitcode);
    }
//    else
}


volatile sig_atomic_t adminMessageEnabled = 1;

void handleSIGUSR1(int signum) {
    adminMessageEnabled = 1;
}


void runBots(Game *game) {

    adminMessageEnabled = 0;

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
            game->bots[game->nBots++] = newBot;
            printf("Bot lançado: %s\n", newBot.name);
        } else {
            perror("Erro ao criar processo para o bot");
        }
    } else {
        printf("Limite de bots alcançado\n");
    }
    adminMessageEnabled = 1;
}

void testBot(Game *game) {
    adminMessageEnabled = 0;

    if (game->nBots > 0) {
        // Seleciona o primeiro bot para teste (índice 0)
        Bot *bot = &game->bots[0];

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
    adminMessageEnabled = 1;

}

void readMaps() {
    adminMessageEnabled = 0;

    printf("Lendo mapas...\n");

    adminMessageEnabled = 1;

}


int processAdminCommand(char *adminCommand, Game *game, GameSettings *gameSettings) {

    if (strcmp(adminCommand, "run_bots") == 0) {
        runBots(game);
    } else if (strcmp(adminCommand, "read_maps") == 0) {
        readMaps();
    } else if (strcmp(adminCommand, "test_bot") == 0) {
        testBot(game);

    }

    else if (strcmp(adminCommand, "users") == 0) {
        printf("\nComando \"users\"\n");
    }

    else if (strncmp(adminCommand, "kick", 4) == 0) {

        char *token = strtok(adminCommand, " ");
        char *targetName = strtok(NULL, " ");
        char *aux;

        if (targetName != NULL) {

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
        printf("\nComando \"end\"\n");
        return -1;
    }

    else {
        printf("Comando do administrador inválido: %s\n", adminCommand);
    }
    return 0;
}

int main() {
    Game game;
    GameSettings gameSettings;

    char adminCommand[50];

    while (1) {

        printf("Insira um comando do administrador: ");
        fgets(adminCommand, sizeof(adminCommand), stdin);
        adminCommand[strcspn(adminCommand, "\n")] = '\0';  // Remova a nova linha

        // Processa o comando do administrador
        if( processAdminCommand(adminCommand, &game, &gameSettings) == -1)
            break;
    }

    return 0;
}





//int main(){
//
//    int pipegen_fd = create_and_open(GENERAL_PIPE, 077);;
//
//    // Pick one
//
//    // Create and open general pipe
//    if (mkfifo(GENERAL_PIPE, 0777) == 0){
//        //do nothing
//    }else if (errno == EEXIST){
//        printf("Ja existe uma instancia Arbitro a executar! So e permitido uma!");
//        exitcode = EXIT_FAILURE; terminate(0);
//    }else{
//        perror("\nERRO: falha ao criar o processo de comunicacao com os clientes");
//        exitcode = EXIT_FAILURE; terminate(0);
//    }
//
//    pipegen_fd = open(GENERAL_PIPE, O_RDWR);
//    if (pipegen_fd == -1){
//        fprintf(stderr, "\nERRO: nao foi possivel abrir o mecanismo de comunicacao geral com os clientes.\n");
//        exitcode = EXIT_FAILURE; terminate(0);
//    }
//
//    return 0;
//}