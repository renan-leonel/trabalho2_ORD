#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ORDEM 5
#define NULO -1
#define BUFFSIZE 500

// uma árvore de ordem 5 possui páginas com, no máximo, 4 chaves e 5 filhos
typedef struct pagina{
    int numero_chaves; // número de chaves contidos na página
    int chaves[ORDEM-1]; // vetor contendo as chaves da página
    int filhos[ORDEM]; // vetor contendo os filhos 
} PAGINA;

// precisamos gravar o rrn(endereço) da página raiz no cabeçalho do arquivo
typedef struct cabecalho{
    int rrn_raiz;
} CABECALHO;

void le_pagina(int rrn, PAGINA *pagina, FILE *btree){ //PAGINA *pagina é um parâmetro de retorno
    int byte_offset = sizeof(CABECALHO) + rrn * sizeof(PAGINA); // calcula o byte-offset da página
    fseek(btree, byte_offset, SEEK_SET); // seek no arquivo btree até o byte-offset calculado
    fread(pagina, sizeof(PAGINA), 1, btree); // le pagina no arquivo btree
}

void escreve_pagina(int rrn, PAGINA *pagina, FILE *btree){ //PAGINA *pagina é um parâmetro de retorno
    int byte_offset = sizeof(CABECALHO) + rrn * sizeof(PAGINA); // calcula o byte-offset da página
    fseek(btree, byte_offset, SEEK_SET); // seek no arquivo btree até o byte-offset calculado
    fwrite(pagina, sizeof(PAGINA), 1, btree); // escreve pagina no arquivo btree
} 

void inicializa_pagina(PAGINA *pagina){
    int i; // contador simples
    pagina.numero_chaves = 0;

    for(i = 0; i < ORDEM-1, i++){
        pagina.chaves[i] = NULO;
        pagina.filhos[i] = NULO;
    }
    pagina.filhos[ORDEM] = NULO;
}

int RRN_novapag(FILE *btree){ // calcula o rrn que a nova página terá no arquivo btree
    int byte_offset;
    int tam_pag;
    int tam_cab;

    fseek(btree, 0, SEEK_END); // reposiciona o ponteiro pro final do arquivo

    byte_offset = ftell(btree);
    tam_pag = sizeof(PAGINA);
    tam_cab = sizeof(CABECALHO);

    return (byte_offset - tam_cab) / tam_pag;
}






void cria_arvore(char *nome_arq){ // nome_arq é o arquivo que será passado pela linha de comando, de onde será realizada a leitura
    FILE *chaves; // arquivo que será utilizado para armazenar o nome_arq
    FILE *btree; // arquivo que será utilizado para armazenar o btree.dat
    int 

    if((chaves = fopen(nome_arq, "rb")) == NULL){ // ve se o arquivo inserido na linha de comando existe
        printf("Erro: arquivo não encontrado!");
        system(pause);
        exit(1);
    }

    if((btree = fopen("btree.dat", "w+b")) ==  NULL){
        printf("Erro na criação do arquivo!");
        system(pause);
        exit(1);
    }



}














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