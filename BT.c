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
    int     ordem;
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
    node->ordem = ordem;

    node->chaves = calloc(ordem, sizeof(int));
    node->registros = calloc(ordem, sizeof(int));
    node->offsetFilhos = calloc(ordem, sizeof(int));

    node->filhos = calloc(ordem+1, sizeof(Node));

    return node;
}

bool ehFolhaNode(Node *node) {
    if (node == NULL) return false;
    return node->ehFolha;
}

int getQtdChavesNode(Node *node) {
    if (node == NULL) return -1;
    return node->qtdChaves;
}

int getOffset(Node *node) { 
    if(node == NULL) return -1;
    return node->offset;
}

int *getChavesNode(Node *node) {
    if (node == NULL) return NULL;
    return node->chaves;
}

int *getRegistrosNode(Node *node) {
    if (node == NULL) return NULL;
    return node->registros;
}

int *getOffsetFilhos(Node *node) {
    if (node == NULL) return NULL;
    return node->offsetFilhos;
}

Node **getFilhosNode(Node *node) {
    if (node == NULL) return NULL;
    return node->filhos;
}

int getOrdemNode(Node *node) {
    if (node == NULL) return -1;
    return node->ordem;
}

void liberaNode(Node *node){
    if (node == NULL) return;

    free(node->chaves);
    free(node->registros);
    free(node->offsetFilhos);

    if (!ehFolhaNode(node)){
        for(int i=0; i<node->qtdChaves; i++){
            liberaNode(node->filhos[i]);
        }
    }
    free(node->filhos);

    free(node);
}

void printNode(Node *node, FILE *arq){
    if (node == NULL) return;

    if(arq == NULL){
        printf("[");
        for(int i=0; i<node->qtdChaves; i++){
            printf("key: %d, ", node->chaves[i]);
        }
        printf("]");
    }else{
        fprintf(arq, "[");
        for(int i=0; i<node->qtdChaves; i++){
            fprintf(arq, "key: %d, ", node->chaves[i]);
        }
        fprintf(arq, "]");
    }
}

//BT
struct BT {
    int     ordem, numNos;
    Node   *raiz;
};

BT *criaBT(int ordem) {
    if (ordem <= 0) return NULL;

    BT* bt = malloc(sizeof(BT));
    bt->ordem = ordem;
    bt->numNos = 0;
    bt->raiz = NULL;

    return bt;
}

void divideNode(Node *raizNova, int ind, Node *raizAntiga, BT *bt) { 
    int ordem = getOrdemNode(raizAntiga)-1;
    bt->numNos++;

    Node *maiores = criaNode(ordem+1, ehFolhaNode(raizAntiga), bt->numNos);

    int indSplit = (ordem-1)/2;
    maiores->qtdChaves = getQtdChavesNode(raizAntiga) - indSplit - 1;
    int limite = indSplit + 1;

    for(int j = 0; j < getQtdChavesNode(maiores); j++){
        maiores->chaves[j] = raizAntiga->chaves[j+limite]; //index j+ordem?
        maiores->registros[j] = raizAntiga->registros[j+limite];
    }

    if(!ehFolhaNode(raizAntiga)) {
        for (int i = 0; i <= getQtdChavesNode(maiores); i++){
            maiores->filhos[i] = raizAntiga->filhos[i+limite];
        }
    }
    raizAntiga->qtdChaves = indSplit;
 
    for(int k = raizNova->qtdChaves+1; k >= ind+1; k--){
        raizNova->filhos[k] = raizNova->filhos[k-1];
    }
    raizNova->filhos[ind+1] = maiores; //c_{i+1}[x] <- z

    for(int l = raizNova->qtdChaves; l > ind; l--){ //>= ind 
        raizNova->chaves[l] = raizNova->chaves[l-1];
        raizNova->registros[l] = raizNova->registros[l-1];
    }

    raizNova->chaves[ind] = raizAntiga->chaves[limite-1];
    raizNova->registros[ind] = raizAntiga->registros[limite-1];
    raizNova->qtdChaves++;
    /*
    diskWrite(raizAntiga);
    diskWrite(maiores);
    diskWrite(raizNova);
    */
}

void insereSemDividir(Node *raiz, int chave, int registro, BT *bt) { 
    int i = raiz->qtdChaves;
    
    if(ehFolhaNode(raiz)){
        while(i>=1 && (chave < raiz->chaves[i-1])){
            raiz->chaves[i] = raiz->chaves[i-1];
            raiz->registros[i] = raiz->registros[i-1];
            i--;
            //raiz->chaves[i] = chave;
            //raiz->registros[i] = registro;     
        }
        //printf("%d ", chave);
        insereChaveRegistro(raiz, chave, registro, i);
        //raiz->qtdChaves++;
        //diskWrite(raiz);

    }else{
        while(i>=1 && (chave < raiz->chaves[i-1])) i--;
        i++;
        ///Node *filho = diskRead(raiz->filhos[i]);
        Node *filho = raiz->filhos[i-1];
       
        // if(getQtdChavesNode(filho) == (bt->ordem - 1)){
        //     divideNode(raiz, i-1, filho, bt);
        //     if(chave > raiz->chaves[i-1]) i++;

        // }
        insereSemDividir(filho, chave, registro, bt); 
        if(getQtdChavesNode(filho)==getOrdemBT(bt))
            divideNode(raiz, i-1,filho, bt);
    }
    
}
void insereChaveRegistro(Node *n, int chave, int registro, int ind){
    if(n == NULL) return;

    n->chaves[ind] = chave;
    n->registros[ind] = registro;
    n->qtdChaves++;
}

void insereBT(BT *bt, int chave, int registro){
    Node* raiz = getRaizBT(bt);
    //printf("%d ", chave);
    if(raiz == NULL){
        bt->numNos++;
        bt->raiz = criaNode(bt->ordem, true, bt->numNos);
        insereChaveRegistro(bt->raiz, chave, registro, 0);
        // escreve no bin
        return;
    }

    insereSemDividir(raiz, chave, registro, bt);
    if((bt->ordem) == raiz->qtdChaves ){ 
        bt->numNos++;
        Node *novo = criaNode(bt->ordem, false, bt->numNos);

        novo->filhos[0] = raiz;
        bt->raiz = novo;
        divideNode(novo, 0, raiz, bt);
        // insereSemDividir(novo, chave, registro, bt);
    }
    // escreve no bin
}

static Node *uneNode(Node *n1, Node *n2) {
    /** os indices estao certos? */
    for(int i = 0; i < getQtdChavesNode(n2); i++) {
        n1->chaves[getQtdChavesNode(n1)+i] = n2->chaves[i];
        n1->registros[getQtdChavesNode(n1)+i] = n2->registros[i];
        //n1->filhos[getQtdChavesNode(n1)] = n2->filhos[i];
        n1->qtdChaves++;
    }

    //liberaNode(n2); /** precisa liberar? ou é certo liberar? */
    return n2;
}

static int getIdxChave(Node *node, int chave) {
    if (node == NULL) return -1;
    for(int i = 0; i < node->qtdChaves; i++) {
        if (node->chaves[i] == chave) return i;
    }
    return -1;
}

static bool podeRemoverDoNode(Node *node) {
    if (node == NULL) return false;

    int ordem = getOrdemNode(node);
    // if (node->qtdChaves >= ordem/2 - 1) return true; //QTDCHAVES - 1 >= (t - (t/2)) - 1
    if (node->qtdChaves-1 >= (ordem - (ordem/2)) - 1) return true;
    return false;
}

static int getMaiorRegistro(Node *node) {
    if (node == NULL) return -1;
    return node->registros[getQtdChavesNode(node) - 1]; 
}

static int getMaiorChave(Node *node) {
    if (node == NULL) return -1;
    return node->chaves[getQtdChavesNode(node) - 1];
}

static int getMenorRegistro(Node *node) {
    if (node == NULL) return -1;
    return node->registros[0];
}

static int getMenorChave(Node *node) {
    if (node == NULL) return -1;
    return node->chaves[0];
}

void trocaConteudos(int *data_1, int *data_2) {
    if (data_1 == NULL || data_2 == NULL) return;
    int aux = *data_1;
    *data_1 = *data_2;
    *data_2 = aux;
}

static void shift(Node *node, int chave, int idxChave) {
    if (!node || chave < 0) return;

    if (idxChave == node->qtdChaves-1) return;

    for(int i = idxChave; i < node->qtdChaves-1; i++) {
        trocaConteudos(&node->chaves[i],    &node->chaves[i+1]);
        trocaConteudos(&node->registros[i], &node->registros[i+1]);
    }
}

void shiftFilhos(Node *node, int idxChave){
    for(int i=idxChave; i<getQtdChavesNode(node); i++){
        node->filhos[idxChave] = node->filhos[idxChave+1];
    }
}

/** ----- Caso 1 ----- */
/** (A) Chave é na folha e a folha tem um minimo de (t/2)-1 chaves, */
static bool remocaoCaso1(Node *node, int chave, int idxChave) {
    if (node == NULL || chave < 0) return false;

    if (ehFolhaNode(node) && podeRemoverDoNode(node)) {
        shift(node, chave, idxChave);
        node->qtdChaves--;
        /** tira do disco o nó "node" */
        return true;
    }
    return false;
}

/** ----- Caso 2 ----- */
/* (A) Se o filho a esquerda do nó a remover com chave k tiver pelo menos t/2 elementos, 
* encontra o maior do filho a esquerda (k'), troca de lugar com k e remove k */
static bool remocaoCaso2(Node *node, int chave, int idxChave) {
    if (node == NULL || chave < 0 || idxChave < 0) return false;

    if (ehFolhaNode(node) == true) return false;

    int ordem = getOrdemNode(node);
    int DIREITA = idxChave+1, ESQUERDA  = idxChave;
    if (podeRemoverDoNode(node->filhos[ESQUERDA])) {
        int registro_troca = getMaiorRegistro(node->filhos[ESQUERDA]);
        int chave_troca = getMaiorChave(node->filhos[ESQUERDA]);
        trocaConteudos(&registro_troca, &node->registros[idxChave]);
        trocaConteudos(&chave_troca,    &node->chaves[idxChave]);
        /** tira do disco o nó "node" */
        node->filhos[ESQUERDA]->qtdChaves--;
        return true;
        
        /* (B) Se for o filho a direita, a mesma coisa, mas com o menorfilho a direita */
    } else if (podeRemoverDoNode(node->filhos[DIREITA])) {
        int registro_troca = getMenorRegistro(node->filhos[DIREITA]);
        int chave_troca = getMenorChave(node->filhos[DIREITA]);
        trocaConteudos(&registro_troca, &node->registros[idxChave]);
        trocaConteudos(&chave_troca, &node->chaves[idxChave]);
        shift(node->filhos[DIREITA], chave_troca, 0);
        /** tira do disco o nó "node" */
        node->filhos[DIREITA]->qtdChaves--;
        return true;
    
    /* (C) Em caso de ambos terem (t/2)-1 chaves, copia as coisa de um nó para para completar o outro */
    } else if (getQtdChavesNode(node->filhos[ESQUERDA]) + getQtdChavesNode(node->filhos[DIREITA]) <= ordem-1 ) {//TALVEZ SEJA(t - t/2) -1 OU (QTDCHAVES ESQ + QTDCHAVES DIR <= ORDEM-1)
        shift(node, chave, idxChave);                                                             // coloquei ordem-1 pq nao sei se estar cheio é uma condicao, imaginei q s
        Node *old = uneNode(node->filhos[ESQUERDA], node->filhos[DIREITA]);
        shiftFilhos(node, idxChave+1);
        node->qtdChaves--;
        liberaNode(old);
        /** tira do disco o nó "node" */
        /** reescreve o node new no lugar do filho a esquerda da chave */
        return true;
    }
    return false;
}

/** ----- Caso 3 ----- */
/**    condicao: x é o nó interno, ci[x] é a subarvore dos filhos de x
 *     (A) (distribuicao) se ci[x] possuir t/2 -1 elementos e possuir um irmao adj com pelo 
 * menos t/2  elementos, move o valor de x para ci[x] e promove uma chave de um dos 
 * irmaos adjacentes
 */
static bool remocaoCaso3(BT *bt, Node *pai, int chave, int idxChave) {
    if (bt == NULL || pai == NULL || chave < 0 || idxChave < 0) return false;

    // encontra o indice do pai q mapeia o filho
    int i = 0;
    for (i = 0; i < getQtdChavesNode(pai) && pai->chaves[i] < chave; i++) { }
    int idx = i - 1;

    Node *left, *mid, *right;
    if (idx == 0) {
        left = mid = pai->filhos[idx];

    } else if (idx == pai->qtdChaves-1) { 
        left = pai->filhos[idx-1]; 
        mid = right = pai->filhos[idx];

    } else {
        left = pai->filhos[idx-1];
        mid = pai->filhos[idx];
        right = pai->filhos[idx+1];
    }

    /**
     * if n[ci[x]] >= t/2 - 1 && n[ci-1[x]] >= t/2 -1:
     *      bota oq ta em x em ci e re-ordena
     *      promove alguem de ci-1 pra x
     *  
     * else if n[ci[x]] >= t/2 - 1 && n[ci+1[x]] >= t/2 -1:
     *      bota oq ta em x em ci e re-ordena
     *      promove alguem de ci+1 pra x
     */

    // pro lado esquerdo
    if (getQtdChavesNode(mid) >= (getOrdemBT(bt)/2 - 1) && getQtdChavesNode(left) >= (getOrdemBT(bt)/2 - 1)) {
        trocaConteudos(&pai->chaves[idx], &mid->chaves[idxChave]);
        trocaConteudos(&pai->registros[idx], &mid->registros[idxChave]);
        // funcao p promover 
        insereSemDividir(pai, chave, chave, bt); // funciona?
        return true;

    // pro lado direito
    } else if (getQtdChavesNode(mid) >= (getOrdemBT(bt)/2 - 1) && getQtdChavesNode(right) >= (getOrdemBT(bt)/2 - 1)) {
        trocaConteudos(&pai->chaves[idx], &mid->chaves[idxChave]);
        trocaConteudos(&pai->registros[idx], &mid->registros[idxChave]);
        // funcao p promover
        insereSemDividir(pai, chave, chave, bt); // funciona?
        return true;
    }
    
    /** 
     *     (B) (concatenacao) se ci[x] e seus irmaos da esquerda e direita tiverem t/2 -1 elementos
     *     deve mover a chave de x para ci[x] e unir ci[x] com um dos irmaos
     */
    /**
     * if (ci[x] && ci-1[x] && ci+1[x] == t/2-1)
     *      bota oq ta em x em ci
     *      junta ci[x] com ci+1[x] ou ci-1[x]
     */

    if (getQtdChavesNode(left) == getQtdChavesNode(mid) == getQtdChavesNode(right) == (getOrdemBT(bt)/2 - 1) ) {
        trocaConteudos(&pai->chaves[idx], &mid->chaves[getQtdChavesNode(mid)-1]); // esses indices estao certos?
        trocaConteudos(&pai->registros[idx], &mid->registros[getQtdChavesNode(mid)-1]);
        uneNode(left, mid);
        return true;
    }
}

void removeBT(BT *bt, int chave) {
    if (bt == NULL || chave < 0) return;

    /** Observações possíveis: Cada nó deve ter pelo menos (t/2) - 1 elementos */
    Node *pai = buscaBT(bt->raiz, NULL, chave, NODE_PAI);
    Node *node = buscaBT(bt->raiz, pai, chave, NODE_CHAVE);
    int idxChave = getIdxChave(node, chave);

    if (remocaoCaso1(node, chave, idxChave));
    else if (remocaoCaso2(node, chave, idxChave));
    else remocaoCaso3(bt, pai, chave, idxChave);
}

Node *buscaBT(Node *node, Node *pai, int chave, int MODO_BUSCA) {
    if(node == NULL) return NULL;

    int i = 0;
    while(i < node->qtdChaves && chave > node->chaves[i]) i++;
    
    switch (MODO_BUSCA) {
        case NODE_CHAVE:
            if(i < node->qtdChaves && chave == node->chaves[i]) return node;
            else if(ehFolhaNode(node)) return NULL;
            else{
                //diskRead(node->filhos[i]);
                return buscaBT(node->filhos[i], node, chave, NODE_CHAVE);
            }

        case NODE_PAI:
            if(i < node->qtdChaves && chave == node->chaves[i]) return pai;
            else if(ehFolhaNode(node)) return NULL;
            else{
                //diskRead(node->filhos[i]);
                return buscaBT(node->filhos[i], node, chave, NODE_PAI);
            }
    }
}

Node *getRaizBT(BT* bt) {
    return bt->raiz;
}

int getOrdemBT(BT *bt) {
    if (bt == NULL) return -1;
    return bt->ordem;
}

int getNumNosBT(BT *bt) {
    if (bt == NULL) return -1;
    return bt->numNos;
}

void liberaBT(BT *bt) {
    if (bt == NULL) return;

    liberaNode(bt->raiz);
    free(bt);
}

void printBT(BT* bt, FILE* arq){
    if(bt == NULL) return;

    if (arq) {
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
                    for(int i=0; i<=tamanho; i++) {
                        insereFila(fila, node->filhos[i]);
                    }
                    numNodesLinha += tamanho + 1;
                }
            }

            n = numNodesLinha;
            fprintf(arq, "\n");
        }
        liberaFila(fila);
        return;
        
    } else {
        printf("\n-- ARVORE B\n");
        //le bin

        Fila* fila = criaFila();
        insereFila(fila, bt->raiz);

        int n = 1; //numero de nodes na mesma linha de impressao
        while (!filaVazia(fila)){
            int numNodesLinha = 0; //numero de nodes na mesma linha de impressao

            for (int j=0; j<n; j++){
                Node* node = (Node*)removeFila(fila);
                printNode(node, NULL);

                if (!ehFolhaNode(node)){
                    int tamanho = node->qtdChaves;
                    for(int i=0; i<=tamanho; i++) {
                        insereFila(fila, node->filhos[i]);
                    }

                    numNodesLinha += tamanho + 1;
                }
            }

            n = numNodesLinha;
            printf("\n");
        }

        liberaFila(fila);
    }
}