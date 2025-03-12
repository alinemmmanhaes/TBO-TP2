#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "BT.h"

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

int getNumElementosNode(Node *node);

void* getChavesNode(Node *node);

bool ehFolhaNode(Node *node){
    return node->ehFolha;
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

void insereBT(BT *bt, int chave, int registro){
    Node* raiz = getRaizBT(bt);

    bt->numNos++;
    if(raiz == NULL){
        bt->raiz = criaNode(bt->ordem, true, bt->numNos);
        // escreve no bin
        return; //retorna mesmo? to em duvida (Aline aqui)
    }

    //insere em nó cheio: split
    //insere em nó não cheio

    // escreve no bin
}

static Node *uneNode(Node *n1, Node *n2);

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

static int *getMaiorFilhoEsquerda(Node *node);
static int *getMenorFilhoDireita(Node *node);

Node *removeBT(BT *bt, int chave) {
    /** 
     * Observações possíveis:
     *      Cada nó deve ter pelo menos (t/2) - 1 elementos
     */

    Node *node = buscaBT(bt, chave);
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
        int *registro = getMaiorFilhoEsquerda(node->filhos[idxChave+1]);
        int aux = *registro;
        *registro = node->registros[idxChave];
        node->registros[idxChave] = aux;
        /** tira do disco o nó "node" */

    } else if (podeRemoverDoNode(node->filhos[idxChave+1])) {
        int *registro = getMenorFilhoDireita(node->filhos[idxChave+1]);
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

bool buscaBT(Node *node, int chave){
    int i = 0;

    while(i < node->qtdChaves && chave > node->chaves[i]) i++;

    if(i < node->qtdChaves && chave == node->chaves[i]) return true;
    else if(ehFolhaNode(node)) return false;
    else{
        //diskRead(node->filhos[i]);
        return buscaBT(node->filhos[i], chave);
    }
}

void printBT(BT* bt, FILE* arq){
    if(bt == NULL) return;
    fprintf(arq, "\n-- ARVORE B\n");

    //le bin
    //printa arvore
}

void liberaBT(BT *bt){
    liberaNode(bt->raiz);
    free(bt);
}

Node *getRaizBT(BT* bt){
    return bt->raiz;
}