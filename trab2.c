#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//A estrutura de árvore de ordem 5 possui páginas com, no máximo, 4 chaves e 5 filhos
typedef struct _pagina{
    int num_chaves; // número de chaves contidos na página
    int chaves[5 - 1]; // vetor contendo as chaves da página
    int filhos[5]; // vetor contendo os filhos 
} PAGINA;

// precisamos gravar o rrn(endereço) da página raiz no cabeçalho do arquivo
typedef struct header{
    int root_rrn;
} HEAD;

//Primeira parte da execução
void start(char*);
void createLeaf(PAGINA*);
void writeLeaf(int, PAGINA*, FILE*);
int readKey(FILE*, int*);
int putKey(int, int*, FILE*);
int put(int, inte, int*, int*, FILE*);
int createRRN(FILE*);
void readLeaf(int, PAGINA*, FILE*);
int keyPosition(int, int, int);
void putKeyAux(int, int, int, int, int*);
void breakLeaf(int, int, PAGINA*, int*, int*, PAGINA*, FILE*);

// Segunda parte de execução
void printResult();
void printTree(FILE*);
void printStatistics(FILE*);
int heightResult(FILE*);

/* 
Função main que recebe os argumentos passados na inicialização do programa
direcionando a execução para a criação da árvore-B ou para a impressão da
mesma. Caso os argumentos estejam incorretos, informa como prosseguir para
a execução do programa.
*/
int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr, "Numero incorreto de argumentos!\n");
        fprintf(stderr, "Como utilizar\n");
        fprintf(stderr, "$ %s -c arquivo_chaves\n", argv[0]);
        fprintf(stderr, "$ %s -p\n", argv[0]);
        exit(1);
    }

    if(strcmp(argv[1], "-c") == 0){ // verifica se foi selecionada a opção "criação"
        printf("Criação de uma Arvore B");
        start(argv[2]); //argv[2] contém o nome do arquivo de chaves 
        printf("Sucesso na criacao da Arvore B!\n");
    } 
    else if(strcmp(argv[1], "-p") == 0){ // verifica se foi selecionada a opção "impressão"
        printResult();
    } 
    else{
        fprintf(stderr, "A opcao \"%s\" nao é suportada!\n", argv[1]);
    }

    return 0;
}

/*
Função para a criação de uma árvore-B na inicialização do programa como criação, deixando 
primeiramente um cabeçalho e uma página raiz vazia. A função ainda faz a chamada
de outra função para a inserção dos demais elementos da árvore. Faz a abertura
dos arquivos à serem utilizados para coleta de dados e salvamento da árvore criada.
*/
void start(char *nome_arq){ // nome_arq é o arquivo que será passado pela linha de comando, de onde será realizada a leitura
    FILE *chaves; // arquivo que será utilizado para armazenar o nome_arq
    FILE *arvore; // arquivo que será utilizado para armazenar o btree.dat

    arvore = fopen("btree.dat", "w + b")
    chaves = fopen(nome_arq, "rb") // ve se o arquivo inserido na linha de comando existe

    int reg_cont, i;
    int chave;
    char buffer[500];
    HEAD cab;
    PAGINA raiz;

    if(chaves == NULL){
        fprintf(stderr, "Arquivo dados.dat não encontrado\n");
        exit(1);
    }
    if(arvore == NULL){
        fprintf(stderr, "Impossivel criar o arquivo btree.dat\n");
        exit(1);
    }

    //o arquivo é criado vazio, acrescentando o cabeçalho e uma página raiz vazia
    cab.root_rrn = 0;
    fwrite(&cab, sizeof(HEAD), 1, arvore);
    createLeaf(&raiz);
    writeLeaf(cab.root_rrn, &raiz, arvore);

    while(readKey(chaves, &chave) > 0){
        if(putKey(chave, &(cab.root_rrn), arvore) == 3){
            fprintf(stderr, "A chave \"%d\" já existe\n", chave);
        }
    }

    fseek(arvore, 0, SEEK_SET);
    fwrite(&cab, sizeof(HEAD), 1, arvore);

    fclose(chaves);
    fclose(arvore);
}

/*
Função que faz a criação de uma nova página com base na raiz fornecida dentro da função
que inicia a criação da árvore em si. Cria a página com os filhos nulos e chaves nulas,
deixando o campo de chaves com zero, ou seja, sem chaves indicadas dentro desta página.
*/
void createLeaf(PAGINA *pag){
    int i;// contador simples

    for(i = 0; i < 5 - 1; i++){
        pag -> chaves[i] = -1;
        pag -> filhos[i] = -1;    
    }
    pag -> filhos[i] = -1;
    pag -> num_chaves = 0;
}

/*
Função que realiza a escrita de uma página dentro do árquivo de árvore, com base no 
RRN fornecido, e pela raíz informada, escrevendo no arquivo da árvore, a página 
recebida por parametro.
*/
void writeLeaf(int rrn, PAGINA *pag, FILE *arvore){
    int offset = sizeof(HEAD) + rrn*sizeof(PAGINA);
    fseek(arvore, offset, SEEK_SET);
    fwrite(pag, sizeof(PAGINA), 1, arvore);
}

/*
Função que recebe um ponteiro de arquivo como parâmetro e utiliza o mesmo para realizar 
uma leitura de qual chave está contida na posição informada pelo ponteiro de arquivo
passado no parâmetro. Retorna como inteiro a chave coletada.
*/
int readKey(FILE *fd, int *chave){
    return fscanf(fd, "%d", chave);
}

/*
Função que recebe a chave a ser inserida, recebe o RRN do local da inserção, e o arquivo
da árvore para realizar as escritas. Utiliza a chamada de uma outra função para poder 
inserir a chave, utilizando do retorno desta para decidir se a pagina da inserção foi dividida ou
não.
*/
int putKey(int chave, int *root_rrn, FILE *arvore){
    PAGINA raiz;
    int chave_pro, filho_d_pro;
    int retorno;

    retorno = put(*root_rrn, chave, &chave_pro, &filho_d_pro, arvore);

    if(retorno == 3){
        return 3;
    } else if(retorno == 1){
        PAGINA novaraiz;
        //a página raiz foi dividida e agora temos uma promoção...
        //devemos criar uma nova raiz, inserir a chave promovida (chave_pro) e
        //respectivos filhos (esquerdo = raiz antiga; direito = filho_d_pro).
        createLeaf(&novaraiz);
        novaraiz.chaves[0] = chave_pro;
        novaraiz.filhos[0] = *root_rrn;
        novaraiz.filhos[1] = filho_d_pro;
        novaraiz.num_chaves = 1;

        //atualiza o root_rrn (obter novo rrn) e escreve a nova página raiz no arquivo.
        *root_rrn = createRRN(arvore);
        writeLeaf(*root_rrn, &novaraiz, arvore);
    }
    return 2;
}

/*
Função utilizada para realiar a verifiação da página onde uma chave será inserida, retornando
se a mesma necessita ser dividida ou não, caso não, apenas retorna e a função anterior realiza
as operações, caso seja necessária a divisão, faz as chamadas para as demais funções onde será
definido o local de inserção da página, e conseguinte, sua real escrita no arquivo por meio de
outra chamada de função especifica para tal ação. 
*/
int put(int rrn_atual, int chave, int *chave_pro, int *filho_d_pro, FILE* arvore){
    PAGINA pag, novapag;
    int chv_pro, rrn_pro;
    int pos;
    int retorno;

    if(rrn_atual == -1){
        *chave_pro = chave;
        *filho_d_pro = -1;
        return 1;
    }

    readLeaf(rrn_atual, &pag, arvore);

    pos = keyPosition(chave, pag.chaves, pag.num_chaves);
    if(pos < pag.num_chaves && pag.chaves[pos] == chave){
        return 3;
    }

    retorno = put(pag.filhos[pos], chave, &chv_pro, &rrn_pro, arvore);

    if(retorno == 0 || retorno == 3){
        return retorno;
    } else{
        if(pag.num_chaves < 5 - 1){
            putKeyAux(chv_pro, rrn_pro, pag.chaves, pag.filhos, &(pag.num_chaves));
            writeLeaf(rrn_atual, &pag, arvore);
            return 0;
        } else{
            breakLeaf(chv_pro, rrn_pro, &pag, chave_pro, filho_d_pro, &novapag, arvore);
            writeLeaf(rrn_atual, &pag, arvore);
            writeLeaf(*filho_d_pro, &novapag, arvore);
            return 1;
        }
    }
}

/*
Função a qual irá fazer seek com o arquivo até o final do mesmo, coletando quantos bytes existem
já inseridos, após, irá calcular qual o RRN utilizado para uma nova insersão com base nas páginas
já existentes. Retorna o numero do RRN à ser criado.
*/
int createRRN(FILE *arvore){ // calcula o rrn que a nova página terá no arquivo btree
    int offset;
    fseek(arvore, 0, SEEK_END);
    offset = ftell(arvore);
    return ((offset - sizeof(HEAD))/sizeof(PAGINA));
}

/*
Função que recebe o RRN de uma página, o ponteiro da página e o ponteiro do arquivo da árvore.
Irá fazer um seek com base no RRN dentro do arquivo da árvore, e após, fará a leitura dos dados
contidos na página à ser consultada, salvando os dados coletados no ponteiro da página.
*/
void readLeaf(int rrn, PAGINA *pag, FILE *arvore){
    int offset = sizeof(HEAD) + rrn*sizeof(PAGINA);
    fseek(arvore, offset, SEEK_SET);
    fread(pag, sizeof(PAGINA), 1, arvore);
}

/*
Função recebendo a chave a ser consultada, as chaves presentes na página, e o numero de chaves
contidas na mesma. Faz uma verificação chave a chave até encontrar a requerida, retornando sua
posição.
*/
int keyPosition(int chave, int chaves[], int num_chaves){ // retorna a posição em que a chave se encontra
    // chaves[] é o vetor que contém todas as chaves
    // chave é a chave que desejamos saber a posição
    // num_chaves é a quantidade total de chaves 

    int i = 0;
    while((i < num_chaves) && (chaves[i] < chave)){
        i++;
    }
    return i;
}

/*
Função que recebe a proxima chave a ser inserida, o proximo RRN, os filhos contidos numa pagina e 
o ponteiro para o numero de chaves da pagina. Faz a chamada de uma função para saber a posição
da chave à ser operada, após isso reduz dentro das chaves anteriores uma posição, e ao final
insere a nova chave na página depois das chaves já existentes e o RRN do filho após a posição 
encontrada.
*/
void putKeyAux(int chv_pro, int rrn_pro, int chaves[], int filhos[], int *num_chaves){
    int pos, k;
    pos = keyPosition(chv_pro, chaves, *num_chaves);
    for(k = *num_chaves; k > pos; k--){
        chaves[k] = chaves[k - 1];
        filhos[k + 1] = filhos[k];
    }
    chaves[pos] = chv_pro;
    filhos[pos + 1] = rrn_pro;
    (*num_chaves)++;
}

/*
Função para a quebra de uma página, criando uma página auxiliar, inserindo a nova chave na pagina
auxiliar, coleta a chave a ser colocada na página promovida, faz a coleta dos filhos e chaves
anteriores à chave removida e deixa-os na página original, faz a copia do filhos após a chave
para a página criada, e, por fim, coloca o RRN do filho da direita da pagina original como sendo
a nova página.
*/
void breakLeaf(int chave_i, int rrn_i, PAGINA *pag, int *chave_pro, int *filho_d_pro, PAGINA *novapag, FILE *arvore){
    int pagaux_chaves[5], pagaux_filhos[5 + 1], pagaux_num_chaves;
    int i, med;

    //copia chaves e filhos da página original para a página auxiliar
    for(i = 0; i < pag->num_chaves; i++){
        pagaux_chaves[i] = pag->chaves[i];
        pagaux_filhos[i] = pag->filhos[i];
    }
    pagaux_filhos[i] = pag->filhos[i];
    pagaux_num_chaves = pag -> num_chaves;

    //insere a chave na página auxiliar
    putKeyAux(chave_i, rrn_i, pagaux_chaves, pagaux_filhos, &(pagaux_num_chaves));

    //a chave a ser promovida da divisão se encontra na mediana da página auxiliar
    med = pagaux_num_chaves/2;
    *chave_pro = pagaux_chaves[med];

    //copia chaves e filhos localizados antes da chave promovida para a página original.
    createLeaf(pag);
    for(i = 0; i < med; i++){
        pag->chaves[i] = pagaux_chaves[i];
        pag->filhos[i] = pagaux_filhos[i];
        pag->num_chaves++;
    }
    pag->filhos[i] = pagaux_filhos[i];

    //copia chaves e filhos localizados após a chave promovida para a nova página
    createLeaf(novapag);
    for(i = med + 1; i < pagaux_num_chaves; i++){
        novapag->chaves[novapag->num_chaves] = pagaux_chaves[i];
        novapag->filhos[novapag->num_chaves] = pagaux_filhos[i];
        novapag->num_chaves++;
    }
    novapag->filhos[novapag->num_chaves] = pagaux_filhos[i];

    //o filho da direita da chave promovida é a nova página
    *filho_d_pro = createRRN(arvore);
}

/*
Função chamada primeiramente na abertura do programa com a opção de impressão dos dados contidos
na árvore-B. Faz a consulta se possivel abrir o arquivo da árvore, caso seja possivel, chama as
funções para executarem as impressões da árvore e de suas estatisticas. Ao fim, fecha o arquivo 
da árvore.
*/
void printResult(){
    FILE *arvore;
    arvore = fopen("btree.dat", "rb")

    if(arvore == NULL){
        fprintf(stderr, "Nao é possivel a abertura do arquivo btree.dat\n");
        exit(1);
    }

    printTree(arvore);
    printStatistics(arvore);

    fclose(arvore);
}

/*
Função que recebe o arquivo da árvore, percorre página a página imprimindo os dados nelas contidos
na tela do usuário. Ao percorrer as páginas, utiliza-se de seeks no arquivo começando com a posição
informada no cabeçalho, apos isso faz seeks com o RRN de cada página incrementado em 1 unidade a
cada impressao realizada até chegar na raiz.
*/
void printTree(FILE *arvore){
    HEAD cab;
    PAGINA pag;
    int i, rrn = 0;
    fseek(arvore, 0, SEEK_SET);
    fread(&cab, sizeof(HEAD), 1, arvore);
    fseek(arvore, sizeof(HEAD), SEEK_SET);
    while(fread(&pag, sizeof(PAGINA), 1, arvore) > 0){
        if(rrn == cab.root_rrn){
            printf("[Pagina Raiz]\n");
        }
        printf("-> Pagina %d\n", rrn);

        printf("-> Chaves: ");
        for(i = 0; i < pag.num_chaves; i++){
            printf("%d | ", pag.chaves[i]);
        }
        printf("%d | ", pag.chaves[i]);

        printf("-> Filhos: ");
        for(i = 0; i < pag.num_chaves; i++){
            printf("%d | ", pag.filhos[i]);
        }
        printf("%d\n", pag.filhos[i]);

        if(rrn == cab.root_rrn){
            printf("\n\n");
        }
        printf("\n");
        rrn++;
    }
}

/*
Função utilizada para a impressão dos dados de uma árvore, recebendo o arquivo da arvore como parametro, 
fazendo a leitura das páginas e incrimentando a quantidade de paginas e de suas chaves. Para impressão,
na altura da arvore faz a chamada de uma outra função que verifica tal dado.
*/
void printStatistics(FILE *arvore){
    int qtd_chaves = 0, qtd_paginas = 0;
    PAGINA pag;

    fseek(arvore, sizeof(HEAD), SEEK_SET);
    while(fread(&pag, sizeof(PAGINA), 1, arvore) > 0){
        qtd_paginas++;
        qtd_chaves += pag.num_chaves;
    }

    printf("\n\n");
    printf("[Caracteristicas da Arvore]\n");
    printf("-> Altura: %d\n", heightResult(arvore));
    printf("-> Quantidade de chaves: %d\n", qtd_chaves);
    printf("-> Quantidade de paginas: %d\n", qtd_paginas);
    printf("-> Taxa de ocupacao: %.2f%%\n", 100*((float)qtd_chaves)/(qtd_paginas*(5 - 1)));
}

/*
Função que verifica a altura de uma árvore com base no arquivo passado por parametro, faz a leitura de
cada folha e incrementa a altura da árvore gradualmente com base nas leituras realizadas das páginas. 
Retorna a altura encontrada para a função de impressão das estatisticas da árvore.
*/
int heightResult(FILE *arvore){
    HEAD cab;
    PAGINA pag;
    int i, rrn = 0, altura = -1;
    fseek(arvore, 0 , SEEK_SET);
    fread(&cab, sizeof(HEAD), 1, arvore);
    rrn = cab.root_rrn;
    while(rrn != -1){
        readLeaf(rrn, &pag, arvore);
        rrn = pag.filhos[0];
        altura++;
    }
    return altura;
}