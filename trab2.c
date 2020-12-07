//////////////////////////////////////////
// Aluno: Henrique Ribeiro Favaro       //
// RA: 115.408                          //
// Aluno: Pedro Henrique de Melo Costa  //
// RA: 112.653                          //
// Aluno: Renan Augusto Leonel          //
// RA: 115.138                          //
////////////////////////////////////////// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ORDER 5
//A estrutura de árvore de ordem 5 possui páginas com, no máximo, 4 chaves e 5 filhos
typedef struct Leaf{
    int keys[ORDER - 1]; // vetor contendo as chaves da página
    int keyAmount; // número de chaves contidos na página
    int children[ORDER]; // vetor contendo os filhos 
} LEAF;

// precisamos gravar o rrn(endereço) da página raiz no cabeçalho do arquivo
typedef struct header{
    int root_rrn;
} HEAD;

//Primeira parte da execução
void start(char*);
void createLeaf(LEAF*);
void writeLeaf(int, LEAF*, FILE*);
int readKey(FILE*, int*);
int putKey(int, int*, FILE*);
int put(int, int, int*, int*, FILE*);
int createRRN(FILE*);
void readLeaf(int, LEAF*, FILE*);
int keyPosition(int, int*, int);
void putKeyAux(int, int, int*, int*, int*);
void breakLeaf(int, int, LEAF*, int*, int*, LEAF*, FILE*);

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
int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "Numero incorreto de argumentos!\n");
        fprintf(stderr, "Como utilizar\n");
        fprintf(stderr, "$ %s -c arquivo_chaves\n", argv[0]);
        fprintf(stderr, "$ %s -p\n", argv[0]);
        exit(1);
    }

    if(strcmp(argv[1], "-c") == 0) // verifica se foi selecionada a opção "criação"
    {
        printf("Criacao de uma Arvore B\n");
        start(argv[2]); //argv[2] contém o nome do arquivo de chaves 
        printf("Sucesso na criacao da Arvore B!\n");
    } 
    else if(strcmp(argv[1], "-p") == 0) // verifica se foi selecionada a opção "impressão"
    { 
        printResult();
    } 
    else
    {
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
void start(char *arquivopassado) // nome_arq é o arquivo que será passado pela linha de comando, de onde será realizada a leitura
{ 
    FILE *chaves; // arquivo que será utilizado para armazenar o nome_arq
    FILE *arvore; // arquivo que será utilizado para armazenar o btree.dat

    arvore = fopen("btree.dat", "w+b");
    chaves = fopen(arquivopassado, "rb"); // ve se o arquivo inserido na linha de comando existe

    int reg_cont, i;
    int key;
    char buffer[500];
    HEAD head_b;
    LEAF root;

    if(chaves == NULL)
    {
        fprintf(stderr, "Arquivo %s nao encontrado\n", arquivopassado);
        exit(1);
    }
    if(arvore == NULL)
    {
        fprintf(stderr, "Impossivel criar o arquivo btree.dat\n");
        exit(1);
    }

    //o arquivo é criado vazio, acrescentando o cabeçalho e uma página raiz vazia
    head_b.root_rrn = 0;
    fwrite(&head_b, sizeof(HEAD), 1, arvore);
    createLeaf(&root);
    writeLeaf(head_b.root_rrn, &root, arvore);

    while(readKey(chaves, &key) > 0)
    {
        if(putKey(key, &(head_b.root_rrn), arvore) == 3)
        {
            fprintf(stderr, "A chave \"%d\" já existe\n", key);
        }
    }

    fseek(arvore, 0, SEEK_SET);
    fwrite(&head_b, sizeof(HEAD), 1, arvore);

    fclose(chaves);
    fclose(arvore);
}

/*
Função que faz a criação de uma nova página com base na raiz fornecida dentro da função
que inicia a criação da árvore em si. Cria a página com os filhos nulos e chaves nulas,
deixando o campo de chaves com zero, ou seja, sem chaves indicadas dentro desta página.
*/
void createLeaf(LEAF *Leaf)
{
    int i;// contador simples

    for(i = 0; i < ORDER - 1; i++)
    {
        Leaf -> keys[i] = -1;
        Leaf -> children[i] = -1;    
    }
    Leaf -> children[i] = -1;
    Leaf -> keyAmount = 0;
}

/*
Função que realiza a escrita de uma página dentro do árquivo de árvore, com base no 
RRN fornecido, e pela raíz informada, escrevendo no arquivo da árvore, a página 
recebida por parametro.
*/
void writeLeaf(int rrn, LEAF *pag, FILE *arvore)
{
    int offset = sizeof(HEAD) + rrn*sizeof(LEAF);
    fseek(arvore, offset, SEEK_SET);
    fwrite(pag, sizeof(LEAF), 1, arvore);
}

/*
Função que recebe um ponteiro de arquivo como parâmetro e utiliza o mesmo para realizar 
uma leitura de qual chave está contida na posição informada pelo ponteiro de arquivo
passado no parâmetro. Retorna como inteiro a chave coletada.
*/
int readKey(FILE *fd, int *chave)
{
    return fscanf(fd, "%d", chave);
}

/*
Função que recebe a chave a ser inserida, recebe o RRN do local da inserção, e o arquivo
da árvore para realizar as escritas. Utiliza a chamada de uma outra função para poder 
inserir a chave, utilizando do retorno desta para decidir se a pagina da inserção foi dividida ou
não.
*/
int putKey(int chave, int *root_rrn, FILE *arvore)
{
    LEAF raiz;
    int promotionKey, sonDPromotion;
    int sendBack;

    sendBack = put(*root_rrn, chave, &promotionKey, &sonDPromotion, arvore);

    if(sendBack == 3)
    {
        return 3;
    } 
    else if(sendBack == 1)
    {
        LEAF newBase;
        //a página raiz foi dividida e agora temos uma promoção...
        //devemos criar uma nova raiz, inserir a chave promovida (chave_pro) e
        //respectivos filhos (esquerdo = raiz antiga; direito = filho_d_pro).
        createLeaf(&newBase);
        newBase.keys[0] = promotionKey;
        newBase.children[0] = *root_rrn;
        newBase.children[1] = sonDPromotion;
        newBase.keyAmount = 1;

        //atualiza o root_rrn (obter novo rrn) e escreve a nova página raiz no arquivo.
        *root_rrn = createRRN(arvore);
        writeLeaf(*root_rrn, &newBase, arvore);
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
int put(int rrn_atual, int chave, int *promotionKey, int *sonDPromotion, FILE* arvore)
{
    LEAF pag, newLeaf;
    int chv_pro, rrn_pro;
    int pos;
    int retorno;

    if(rrn_atual == -1)
    {
        *promotionKey = chave;
        *sonDPromotion = -1;
        return 1;
    }

    readLeaf(rrn_atual, &pag, arvore);

    pos = keyPosition(chave, pag.keys, pag.keyAmount);
    if(pos < pag.keyAmount && pag.keys[pos] == chave)
    {
        return 3;
    }

    retorno = put(pag.children[pos], chave, &chv_pro, &rrn_pro, arvore);

    if(retorno == 0 || retorno == 3)
    {
        return retorno;
    } 
    else
    {
        if(pag.keyAmount < ORDER - 1)
        {
            putKeyAux(chv_pro, rrn_pro, pag.keys, pag.children, &(pag.keyAmount));
            writeLeaf(rrn_atual, &pag, arvore);
            return 0;
        } 
        else
        {
            breakLeaf(chv_pro, rrn_pro, &pag, promotionKey, sonDPromotion, &newLeaf, arvore);
            writeLeaf(rrn_atual, &pag, arvore);
            writeLeaf(*sonDPromotion, &newLeaf, arvore);
            return 1;
        }
    }
}

/*
Função a qual irá fazer seek com o arquivo até o final do mesmo, coletando quantos bytes existem
já inseridos, após, irá calcular qual o RRN utilizado para uma nova insersão com base nas páginas
já existentes. Retorna o numero do RRN à ser criado.
*/
int createRRN(FILE *arvore) // calcula o rrn que a nova página terá no arquivo btree
{ 
    int offset;
    fseek(arvore, 0, SEEK_END);
    offset = ftell(arvore);
    return (offset - sizeof(HEAD))/sizeof(LEAF);
}

/*
Função que recebe o RRN de uma página, o ponteiro da página e o ponteiro do arquivo da árvore.
Irá fazer um seek com base no RRN dentro do arquivo da árvore, e após, fará a leitura dos dados
contidos na página à ser consultada, salvando os dados coletados no ponteiro da página.
*/
void readLeaf(int rrn, LEAF *pag, FILE *arvore)
{
    int offset = sizeof(HEAD) + rrn*sizeof(LEAF);
    fseek(arvore, offset, SEEK_SET);
    fread(pag, sizeof(LEAF), 1, arvore);
}

/*
Função recebendo a chave a ser consultada, as chaves presentes na página, e o numero de chaves
contidas na mesma. Faz uma verificação chave a chave até encontrar a requerida, retornando sua
posição.
*/
int keyPosition(int chave, int chaves[], int keyAmount) // retorna a posição em que a chave se encontra
{ 
    // chaves[] é o vetor que contém todas as chaves
    // chave é a chave que desejamos saber a posição
    // keyAmount é a quantidade total de chaves 

    int i = 0;
    while(i < keyAmount && chaves[i] < chave)
    {
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
void putKeyAux(int chv_pro, int rrn_pro, int chaves[], int children[], int *keyAmount)
{
    int pos, k;
    pos = keyPosition(chv_pro, chaves, *keyAmount);
    for(k = *keyAmount; k > pos; k--)
    {
        chaves[k] = chaves[k - 1];
        children[k + 1] = children[k];
    }
    chaves[pos] = chv_pro;
    children[pos + 1] = rrn_pro;
    (*keyAmount)++;
}

/*
Função para a quebra de uma página, criando uma página auxiliar, inserindo a nova chave na pagina
auxiliar, coleta a chave a ser colocada na página promovida, faz a coleta dos filhos e chaves
anteriores à chave removida e deixa-os na página original, faz a copia do filhos após a chave
para a página criada, e, por fim, coloca o RRN do filho da direita da pagina original como sendo
a nova página.
*/
void breakLeaf(int iKey, int iRRN, LEAF *pag, int *promotionKey, int *sonDPromotion, LEAF *newLeaf, FILE *arvore)
{
    int nextKeys[ORDER], nextChildren[ORDER + 1], nextKeyNumber;
    int i, middle;

    //copia chaves e filhos da página original para a página auxiliar
    for(i = 0; i < pag->keyAmount; i++)
    {
        nextKeys[i] = pag->keys[i];
        nextChildren[i] = pag->children[i];
    }
    nextChildren[i] = pag->children[i];
    nextKeyNumber = pag -> keyAmount;

    //insere a chave na página auxiliar
    putKeyAux(iKey, iRRN, nextKeys, nextChildren, &(nextKeyNumber));

    //a chave a ser promovida da divisão se encontra na mediana da página auxiliar
    middle = nextKeyNumber/2;
    *promotionKey = nextKeys[middle];

    //copia chaves e filhos localizados antes da chave promovida para a página original.
    createLeaf(pag);
    for(i = 0; i < middle; i++)
    {
        pag->keys[i] = nextKeys[i];
        pag->children[i] = nextChildren[i];
        pag->keyAmount++;
    }
    pag->children[i] = nextChildren[i];

    //copia chaves e filhos localizados após a chave promovida para a nova página
    createLeaf(newLeaf);
    for(i = middle + 1; i < nextKeyNumber; i++)
    {
        newLeaf->keys[newLeaf->keyAmount] = nextKeys[i];
        newLeaf->children[newLeaf->keyAmount] = nextChildren[i];
        newLeaf->keyAmount++;
    }
    newLeaf->children[newLeaf->keyAmount] = nextChildren[i];

    //o filho da direita da chave promovida é a nova página
    *sonDPromotion = createRRN(arvore);
}

/*
Função chamada primeiramente na abertura do programa com a opção de impressão dos dados contidos
na árvore-B. Faz a consulta se possivel abrir o arquivo da árvore, caso seja possivel, chama as
funções para executarem as impressões da árvore e de suas estatisticas. Ao fim, fecha o arquivo 
da árvore.
*/
void printResult()
{
    FILE *arvore;
    arvore = fopen("btree.dat", "rb");

    if(arvore == NULL)
    {
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
void printTree(FILE *arvore)
{
    HEAD head_b;
    LEAF leaf;
    int i, rrn = 0;
    fseek(arvore, 0, SEEK_SET);
    fread(&head_b, sizeof(HEAD), 1, arvore);
    fseek(arvore, sizeof(HEAD), SEEK_SET);
    while(fread(&leaf, sizeof(LEAF), 1, arvore) > 0)
    {
        if(rrn == head_b.root_rrn)
        {
            printf("\n\n\n[----------Pagina Raiz----------]\n");
        }
        printf("-> Pagina %d\n", rrn);

        printf("-> Chaves: ");
        for(i = 0; i < leaf.keyAmount-1; i++)
        {
            printf("%d | ", leaf.keys[i]);
        }
        printf("%d | \n", leaf.keys[i]);

        printf("-> Filhos: ");
        for(i = 0; i < leaf.keyAmount; i++)
        {
            printf("%d | ", leaf.children[i]);
        }
        if(rrn == head_b.root_rrn)
        {
            printf("%d\n", leaf.children[i]);
        } else
        {
            printf("%d\n\n", leaf.children[i]);
        }
        
        if(rrn == head_b.root_rrn)
        {
            printf("[-------------------------------]\n");
            printf("\n\n\n\n");
        }
        rrn++;
    }
}

/*
Função utilizada para a impressão dos dados de uma árvore, recebendo o arquivo da arvore como parametro, 
fazendo a leitura das páginas e incrimentando a quantidade de paginas e de suas chaves. Para impressão,
na altura da arvore faz a chamada de uma outra função que verifica tal dado.
*/
void printStatistics(FILE *arvore)
{
    int keyAmount = 0, leafAmount = 0;
    LEAF leaf;

    fseek(arvore, sizeof(HEAD), SEEK_SET);
    while(fread(&leaf, sizeof(LEAF), 1, arvore) > 0)
    {
        leafAmount++;
        keyAmount += leaf.keyAmount;
    }

    printf("\n\n");
    printf("[Caracteristicas da Arvore]\n");
    printf("-> Altura: %d\n", heightResult(arvore));
    printf("-> Quantidade de chaves: %d\n", keyAmount);
    printf("-> Quantidade de paginas: %d\n", leafAmount);
    printf("-> Taxa de ocupacao: %.2f%%\n", 100*((float)keyAmount)/(leafAmount*(ORDER - 1)));
}

/*
Função que verifica a altura de uma árvore com base no arquivo passado por parametro, faz a leitura de
cada folha e incrementa a altura da árvore gradualmente com base nas leituras realizadas das páginas. 
Retorna a altura encontrada para a função de impressão das estatisticas da árvore.
*/
int heightResult(FILE *arvore)
{
    HEAD head_b;
    LEAF leaf;
    int i, rrn = 0, height = -1;
    fseek(arvore, 0 , SEEK_SET);
    fread(&head_b, sizeof(HEAD), 1, arvore);
    rrn = head_b.root_rrn;
    while(rrn != -1)
    {
        readLeaf(rrn, &leaf, arvore);
        rrn = leaf.children[0];
        height++;
    }
    return height;
}