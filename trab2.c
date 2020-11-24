#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ORDEM 5
#define NULO -1
#define BUFFSIZE 500

int main(int argc, char *argv[]){
        if(argc < 2){
        fprintf(stderr, "Erro: numero incorreto de argumentos!\n");
        fprintf(stderr, ">Modo de uso:\n");
        fprintf(stderr, "$ %s -c arquivo_chaves\n", argv[0]);
        fprintf(stderr, "$ %s -p\n", argv[0]);

        exit(1);
    }

    if(strcmp(argv[1], "-c") == 0){  // verifica se foi selecionada a opção "criação"

    }
    else if(strcmp(argv[1], "-p") == 0){  // verifica se foi selecionada a opção "impressão"

    }
    else{
        fprintf(stderr, "Erro!");
    }

    return 0;


    return 0
}