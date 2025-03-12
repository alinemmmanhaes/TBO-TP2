#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "BT.h"
#include "fila.h"

//Node
struct Node {
    bool    ehFolha;
    int     qtdChaves;
    int     offset;
    int    *chaves;
    int    *registros;
    int    *offsetFilhos;
    Node  **filhos;
};  

Node *criaNode(int ordem, bool ehFolha, int offset){
    Node* node = malloc(sizeof(Node));

    node->ehFolha = ehFolha;
    node->qtdChaves = 0;
    node->offset = offset;

    node->chaves = calloc(ordem-1, sizeof(int));
    node->registros = calloc(ordem-1, sizeof(int));
    node->offsetFilhos = calloc(ordem-1, sizeof(int));

    node->filhos = calloc(ordem, sizeof(Node));

    return node;
}

int getNumElementosNode(Node *node) { }

int getOffset(Node *node) { 
    if(node) return node->offset;
    return 0;
}

void* getChavesNode(Node *node) { }

bool ehFolhaNode(Node *node){
    return node->ehFolha;
}

void liberaNode(Node *node){
    free(node->chaves);
    free(node->registros);
    free(node->offsetFilhos);

    if(!ehFolhaNode(node)){
        for(int i=0; i<node->qtdChaves; i++){
            liberaNode(node->filhos[i]);
        }
    }
    free(node->filhos);

    free(node);
}

void printNode(Node *node, FILE *arq){
    fprintf(arq, "[");
    for(int i=0; i<node->qtdChaves; i++){
        fprintf(arq, "key: %d, ", node->chaves[i]);
    }
    fprintf(arq, "]");
}

//BT
struct BT {
    int     ordem, numNos;
    Node    *raiz;
};

BT *criaBT(int ordem) {
    BT* bt = malloc(sizeof(BT));
    bt->ordem = ordem;
    bt->numNos = 0;
    bt->raiz = NULL;

    return bt;
}
void *divideNode(Node *raizNova,int ind, Node *raizAntiga) { 
    
    int ordem = getOrdem(raizAntiga);
    Node *maiores = criaNode(ordem, ehFolhaNode(raizAntiga),getOffset(raizNova)+1);
   
   
    maiores->chaves = ordem/2;

    for(int j = 0;j<(ordem-1);j++){
        maiores->chaves[j] = raizAntiga->chaves[j+ordem];
    }
    if(!ehFolhaNode(raizAntiga)){
        for (int i = 0; i < ordem; i++){
            maiores->filhos[i] = raizAntiga->filhos[i+ordem];
        }
        raizAntiga->chaves = ordem - 1;
    }
    for(int k = raizNova->chaves+1;k>=ind+1;k--){
        raizNova->filhos[k+1] = raizNova->filhos[k];
    }
    for(int l = raizNova->chaves;l>=ind;l--){
        raizNova->chaves[l+1] = raizNova->chaves[l];
    }
    raizNova->chaves[ind] = raizAntiga->chaves[ordem];
    raizNova->qtdChaves++;
    /*
    diskWrite(raizAntiga);
    diskWrite(maiores);
    diskWrite(raizNova);
    */
}

void *insereSemDividir(Node *raiz ,int chave) { 
    int i = raiz->qtdChaves;
    
    if(ehFolhaNode(raiz)){
        while(i>=1 && (chave < raiz->chaves[i])){
            raiz->chaves[i + 1] = raiz->chaves[i];
            i--;
            raiz->chaves[i + 1] = chave;
            //não entendi pq aqui não troca os registros de lugar;      
        }
        raiz->qtdChaves++;
        //diskWrite(raiz);
    }else{
        while(i>=1 && (chave < raiz->chaves[i]))i--;
        i++;
        ///Node *filho = diskRead(raiz->filhos[i]);
        Node *filho = raiz->filhos[i];
        if(filho->offsetFilhos == (getOrdem(filho) - 1)){
            divideNode(raiz,i,filho);
            if(chave>raiz->chaves[i]) i++;
        }
        else{
            insereSemDividir(filho,chave);
        }
    }
}

void insereBT(BT *bt, int chave, int registro){
    Node* raiz = getRaizBT(bt);
    int i = 0;
   
    if(raiz == NULL){
        bt->raiz = criaNode(bt->ordem, true, bt->numNos);
        // escreve no bin
        return; //retorna mesmo? to em duvida (Aline aqui)
    }
    if((bt->ordem) - 1 == raiz->qtdChaves ){ 
        bt->numNos++;
        Node *novo = criaNode(bt->ordem, false, bt->numNos);

        novo->filhos[0] = raiz;
        novo = divideNode(novo, 0, raiz);
        bt->numNos++;
        for(int j = 0; j<novo->qtdChaves;j++){
            if(novo->chaves[j]>chave) insereSemDividir(novo, chave);
        }
        raiz = novo;

    }
    else{
        insereSemDividir(raiz, chave);
    }
    //insere em nó f: split
    //insere em nó não cheio

    // escreve no bin
}

static Node *uneNode(Node *n1, Node *n2) { }

static int getIdxChave(Node *node, int chave) {
    if (node == NULL) return -1;
    for(int i = 0; i < node->qtdChaves; i++) {
        if (node->chaves[i] == chave) return i;
    }
    return -1;
}

static int getOrdem(Node *node) {
    if (node == NULL) return 0;
    return sizeof(node->registros)/sizeof(int);
}

static bool podeRemoverDoNode(Node *node) {
    if (node == NULL) return false;

    if (node->qtdChaves >= getOrdem(node)/2 - 1) return true;
    return false;
}

static int *getMaiorRegistroFilhoEsquerda(Node *node) { }

static int *getMenorRegistroFilhoDireita(Node *node) { }

Node *removeBT(BT *bt, int chave) {
    /** 
     * Observações possíveis:
     *      Cada nó deve ter pelo menos (t/2) - 1 elementos
     */

    Node *node = buscaBT(bt->raiz, chave);
    int idxChave = getIdxChave(node, chave);
    /** ----- Caso 1 ----- */
    /** Chave é na folha e a folha tem um minimo de (t/2)-1 chaves, */
    if (ehFolhaNode(node) && podeRemoverDoNode(node)) {
        node->qtdChaves--;
        /** tem q remover essa chave de alguma maneira
         * o problema é como: chaves podem ser negativas? se sim, calloc n deve funcionar
         */
        /** tira do disco o nó "node" */
        return node;
    }

    /** ----- Caso 2 ----- */
    /**     condicao: chave esta em um nó interno
     *      (A) Se o filho a esquerda do nó a remover com chave k tiver pelo menos t/2 elementos, 
     * encontra o maior do filho a esquerda (k'), troca de lugar com k e remove k
     * 
     *      (B) Se for o filho a direita, a mesma coisa, mas com o menorfilho a direita
     * 
     *      (C) Em caso de ambos terem (t/2)-1 chaves, copia as coisa de um nó para para completar o outro
     */

    /** 
     * ADENDO: pode haver vai ter alguma inconsistencia por possível erro nos ponteiros 
     * ou enderecos em ambos casos */
    if (podeRemoverDoNode(node->filhos[idxChave]) && podeRemoverDoNode(node->filhos[idxChave+1])) {
        /** ADENDO: como saber se é folha? */
        Node *new = uneNode(node->filhos[idxChave], node->filhos[idxChave+1]);
        /** tira do disco o nó "node" */
        /** reescreve o node new no lugar do filho a esquerda da chave */

    } else if (podeRemoverDoNode(node->filhos[idxChave])) {
        int *registro = getMaiorRegistroFilhoEsquerda(node->filhos[idxChave+1]);
        int aux = *registro;
        *registro = node->registros[idxChave];
        node->registros[idxChave] = aux;
        /** tira do disco o nó "node" */

    } else if (podeRemoverDoNode(node->filhos[idxChave+1])) {
        int *registro = getMenorRegistroFilhoDireita(node->filhos[idxChave+1]);
        int aux = *registro;
        *registro = node->registros[idxChave];
        node->registros[idxChave] = aux;
        /** tira do disco o nó "node" */
    }

     /** ----- Caso 3 ----- */
     /**    condicao: x é o nó interno, ci[x] é a subarvore dos filhos de x
      *     (A) (distribuicao) se ci[x] possuir t/2 -1 elementos e possuir um irmao adj com pelo 
      * menos t/2  elementos, move o valor de x para ci[x] e promove uma chave de um dos 
      * irmaos adjacentes
      * 
      *     (B) (concatenacao) se ci[x] e seus irmaos da esquerda e direita tiverem t/2 -1 elementos
      *     deve mover a chave de x para ci[x] e unir ci[x] com um dos irmaos
      */

    return node;
}

Node *buscaBT(Node *node, int chave){
    if(node == NULL) return NULL;
    int i = 0;

    while(i < node->qtdChaves && chave > node->chaves[i]) i++;

    if(i < node->qtdChaves && chave == node->chaves[i]) return node;
    else if(ehFolhaNode(node)) return NULL;
    else{
        //diskRead(node->filhos[i]);
        return buscaBT(node->filhos[i], chave);
    }
}

void printBT(BT* bt, FILE* arq){
    if(bt == NULL) return;

    fprintf(arq, "\n-- ARVORE B\n");
    //le bin

    Fila* fila = criaFila();
    insereFila(fila, bt->raiz);

    int n = 1; //numero de nodes na mesma linha de impressao
    while(!filaVazia(fila)){
        int numNodesLinha = 0; //numero de nodes na mesma linha de impressao

        for(int j=0; j<n; j++){
            Node* node = (Node*)removeFila(fila);
            printNode(node, arq);

            if(!ehFolhaNode(node)){
                int tamanho = node->qtdChaves;
                for(int i=0; i<=tamanho; i++){
                    insereFila(fila, node->filhos[i]);
                }
                numNodesLinha += tamanho + 1;
            }
        }

        n = numNodesLinha;
        fprintf(arq, "\n");
    }
    liberaFila(fila);
}

void liberaBT(BT *bt){
    liberaNode(bt->raiz);
    free(bt);
}

Node *getRaizBT(BT* bt){
    return bt->raiz;
}