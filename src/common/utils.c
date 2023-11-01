#include "utils.h"

int randomNum(int min, int max)
{
	//Retorna um numero random entre min e max (inclusive)
    return min + rand()%(max-min+1);
}


int sendSignal(pid_t pid, int signum)
{
    /*
    Recebe:
        - PID do processo alvo
        - codigo do sinal a enviar
    Devolve:
        - 0 em caso de sucesso
        - (-1) se ocorrer falha. Neste caso, errno e inicializado de acordo com o manual de sigqueue()
    */

    union sigval val = {
        .sival_int = 0      //necessario para o sigqueue
    };

    errno = 0;  //faz reset para possibilitar debug de eventual erros

    return sigqueue(pid, signum, val);     //envia SIGINT para os clientes
}


void delete_char(char *s,char ch)
{
	// Para remover todos os caracteres iguais a variavel ch

    for (int i = 0; s[i] != '\0'; i++){

    	//Se encontrar o caracter indicado:
        if ( s[i] == ch ){
        	//Elimina esse caracter e reorganiza a string:
            for (int j = i; s[j] != '\0'; j++)
                s[j] = s[j+1];

            i--;	//Subtrai ao iterador para verificar o novo caracter
            		//que foi agora escrito na posicao atual
        }
    }
}


int parseNum(char *str, int min, int max){
    /*
    Recebe:
        >  string a converter para inteiro;
        >  minimo permitido para o valor lido (inclusive);
        >  maximo permitido para o valor lido (inclusive).
    Devolve:
        >  o numero inteiro extraido;
        >  min-1   - em caso de o valor inserido ser demasiado grande ou nao haver nenhum digito na string.
    */

    char *endptr;
    long num = strtol(str, &endptr, 10);

    if (errno == ERANGE){
        printf("\nERRO: valor demasiado longo.\n\n\n");
        errno = 0;      //Fazer reset ao codigo de erro
        return min-1;
    }
    else if (endptr == str){
        printf("\nERRO: nao foi encontrado nenhum valor numerico! :%s:\n\n\n", str);
        return min-1;
    }
    else if (num < min || num > max){
        printf("\nERRO: o valor tem de estar no intervalo [%d,%d].\n\n\n", min, max);
        return min-1;
    }

    //Caso seja valido
    return (int)num;    //esta conversao de long para int nunca falha pois o maximo possivel e INT_MAX
}