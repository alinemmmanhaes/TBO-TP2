#include <stdbool.h>

#include "BT.h"


struct Node {
    bool    ehFolha;
    int     qtdChaves;
    int     offset;
    int    *chaves;
    int    *registros;
    Node  **filhos;
};

Node *criaNode();

void liberaNode(Node *node);

int getNumElementosNode(Node *node);

void* getChavesNode(Node *node);

bool ehFolhaNode(Node *node);


 struct BT {
    int     ordem, numNos;
    Node    *raiz;
 };

BT *criaBT();

void insereBT(BT *bt, int chave, int registro);

static bool podeRemoverNode(Node *node);

Node *removeBT(BT *bt, int chave) {
    /** 
     * Observações possíveis:
     *      Cada nó deve ter pelo menos (t/2) - 1 elementos
     */

    Node *encontrado = buscaBT(bt, chave);
    /** ----- Caso 1 ----- */
    /** Chave é na folha e a folha tem um minimo de (t/2)-1 chaves, */
    if (ehFolhaNode(encontrado) && podeRemoverNode(encontrado)) {
        /** tira do disco */
        return encontrado;
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
    

     /** ----- Caso 3 ----- */
     /**    condicao: x é o nó interno, ci[x] é a subarvore dos filhos de x
      *     (A) (distribuicao) se ci[x] possuir t/2 -1 elementos e possuir um irmao adj com pelo 
      * menos t/2  elementos, move o valor de x para ci[x] e promove uma chave de um dos 
      * irmaos adjacentes
      * 
      *     (B) (concatenacao) se ci[x] e seus irmaos da esquerda e direita tiverem t/2 -1 elementos
      *     deve mover a chave de x para ci[x] e unir ci[x] com um dos irmaos
      */




}

Node *buscaBT(BT *bt, int chave);

void liberaBT(BT *bt);