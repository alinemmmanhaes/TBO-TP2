#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "BT.h"
#include "fila.h"

void removeNode(BT *bt, Node *node, Node *pai, int chave, FILE* arq);

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

Node *liberaNode(Node *node){
    if (node == NULL) return NULL;

    free(node->chaves);
    free(node->registros);
    free(node->offsetFilhos);

    if (!ehFolhaNode(node)){
        for(int i = 0; i <= node->qtdChaves; i++){
            node->filhos[i] = liberaNode(node->filhos[i]);
        }
    }
    free(node->filhos);

    free(node);
    return NULL;
}

int calculaOffset(int positionDisk, int ordem){
    int soma = positionDisk*(2*sizeof(int)+sizeof(bool)+(sizeof(int)*(ordem-1))+(sizeof(int)*(ordem))+(sizeof(int)*(ordem-1)));
    return soma;
}

void printNode(Node *node, FILE *arq){
    if (node == NULL) return;

    if(arq == NULL){
        printf("[");
        for(int i=0; i<node->qtdChaves; i++){
            printf("key: %d, ", node->chaves[i]);
        }
        printf("]");

    } else {
        fprintf(arq, "[");
        for(int i = 0; i < node->qtdChaves; i++){
            fprintf(arq, "key: %d, ", node->chaves[i]);
        }
        fprintf(arq, "]");
    }
}
static int getIdxChave(Node *node, int chave) {
    if (node == NULL) return -1;
    for(int i = 0; i < node->qtdChaves; i++) {
        if (node->chaves[i] == chave) return i;
    }
    return -1;
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

void divideNode(Node *raizNova, int ind, Node *raizAntiga, BT *bt, FILE *arq) { 
    int ordem = getOrdemNode(raizAntiga)-1;
    
    Node *maiores = criaNode(ordem+1, ehFolhaNode(raizAntiga), bt->numNos);
    bt->numNos++;

    int indSplit = (ordem-1) - ((ordem-1)/2);
    maiores->qtdChaves = getQtdChavesNode(raizAntiga) - indSplit - 1;
    int limite = indSplit + 1;

    for(int j = 0; j < getQtdChavesNode(maiores); j++){
        maiores->chaves[j] = raizAntiga->chaves[j+limite];
        maiores->registros[j] = raizAntiga->registros[j+limite];
    }

    Node *aux = NULL;
    if(!ehFolhaNode(raizAntiga)) {
        for (int i = 0; i <= getQtdChavesNode(maiores); i++){
            aux =  maiores->filhos[i];
            maiores->filhos[i] = raizAntiga->filhos[i+limite];
            maiores->offsetFilhos[i] =  raizAntiga->filhos[i+limite]->offset;
            raizAntiga->filhos[i+limite] = aux;
        }
    }
    raizAntiga->qtdChaves = indSplit;
 
    for(int k = raizNova->qtdChaves+1; k >= ind+1; k--) {
        raizNova->filhos[k] = raizNova->filhos[k-1];
        raizNova->offsetFilhos[k]= raizNova->filhos[k-1]->offset;
    }
    raizNova->filhos[ind+1] = maiores;
    raizNova->offsetFilhos[ind+1] = maiores->offset;

    for(int l = raizNova->qtdChaves; l > ind; l--){
        raizNova->chaves[l] = raizNova->chaves[l-1];
        raizNova->registros[l] = raizNova->registros[l-1];
    }

    raizNova->chaves[ind] = raizAntiga->chaves[limite-1];
    raizNova->registros[ind] = raizAntiga->registros[limite-1];
    raizNova->qtdChaves++;

    diskWrite(raizAntiga, bt->ordem, arq);
    diskWrite(raizNova, bt->ordem, arq);
    diskWrite(maiores, bt->ordem, arq);
}

void insereSemDividir(Node *raiz, int chave, int registro, BT *bt, FILE *arq) { 
    int i = raiz->qtdChaves;
    
    if(ehFolhaNode(raiz)){
        while(i>=1 && (chave < raiz->chaves[i-1])) {
            raiz->chaves[i] = raiz->chaves[i-1];
            raiz->registros[i] = raiz->registros[i-1];
            i--;     
        }
        insereChaveRegistro(raiz, chave, registro, i);
        diskWrite(raiz, raiz->ordem, arq);

    }else{
        while(i>=1 && (chave < raiz->chaves[i-1])) i--;
        i++;
        // Node *filho = raiz->filhos[i-1];
        Node *filho = diskRead(raiz->offsetFilhos[i-1], getOrdemBT(bt), arq);

        insereSemDividir(filho, chave, registro, bt, arq); 
        if(getQtdChavesNode(filho)==getOrdemBT(bt))
            divideNode(raiz, i-1, filho, bt, arq);
    }
    
}
void insereChaveRegistro(Node *n, int chave, int registro, int ind){
    if(n == NULL) return;

    n->chaves[ind] = chave;
    n->registros[ind] = registro;
    n->qtdChaves++;
}

void insereBT(BT *bt, int chave, int registro, FILE *arq){
    Node* raiz = getRaizBT(bt);
    if(raiz == NULL){
        bt->raiz = criaNode(bt->ordem, true, bt->numNos);
        bt->numNos++;
        insereChaveRegistro(bt->raiz, chave, registro, 0);
        diskWrite(bt->raiz, bt->ordem, arq);
        return;
    }

    Node *busca = buscaBT(raiz, NULL, chave, NODE_CHAVE, arq);
    if(busca != NULL){
        int idx = getIdxChave(busca,chave);
        busca->registros[idx] = registro;
        diskWrite(busca, getOrdemBT(bt) ,arq);
        return;
    }

    insereSemDividir(raiz, chave, registro, bt, arq);
    if((bt->ordem) == raiz->qtdChaves ){ 
        Node *novo = criaNode(bt->ordem, false, bt->numNos);
        bt->numNos++;

        novo->filhos[0] = raiz;
        novo->offsetFilhos[0] = raiz->offset;
        bt->raiz = novo;
        divideNode(novo, 0, raiz, bt, arq);
    }
    //diskWrite(bt->raiz,bt->ordem, arq);
}

static Node *uneNode(Node *n1, Node *n2, int chave, int registro) {
    int qtdChavesN1 = getQtdChavesNode(n1), qtdChavesN2 = getQtdChavesNode(n2);

    n1->chaves[qtdChavesN1] = chave;
    n1->registros[qtdChavesN1] = registro;

    int aux = 0;
    for(int i = 1; i <= qtdChavesN2; i++) {
        aux = n1->chaves[qtdChavesN1+i];
        n1->chaves[qtdChavesN1+i] = n2->chaves[i-1];
        n2->chaves[i-1] = aux;

        aux = n1->registros[qtdChavesN1+i];
        n1->registros[qtdChavesN1+i] = n2->registros[i-1];
        n2->registros[i-1] = n1->registros[qtdChavesN1+i];
    }

    if (ehFolhaNode(n1) == false) {
        Node *aux = NULL;
        for (int i = 1; i <= qtdChavesN2+1; i++) {
            aux = n1->filhos[i+qtdChavesN1];
            n1->filhos[i+qtdChavesN1] = n2->filhos[i-1];
            n2->filhos[i-1] = aux;
        }
    }

    if (ehFolhaNode(n2) == false){
        for(int i = 0; i <= n2->qtdChaves; i++) {
            n2->filhos[i] = NULL;
        }
    }

    n1->qtdChaves += n2->qtdChaves+1;
    return n2;
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

static void trocaConteudos(int *data_1, int *data_2) {
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

static void shiftFilhos(Node *node, int idxChave){
    Node *aux = NULL;
    for(int i = idxChave; i < getQtdChavesNode(node); i++){
        aux = node->filhos[i];
        node->filhos[i] = node->filhos[i+1];
        node->offsetFilhos[i] = node->filhos[i+1]->offset;
        node->filhos[i+1] = aux;
    }
}

static void setChaveRegistroTroca(Node *node, int chave, int registro, int idx) {
    if (node == NULL) return;
    node->chaves[idx] = chave;
    node->registros[idx] = registro;
}

static void insereNode(Node *node, int chave, int registro) { 
    int i = getQtdChavesNode(node);
    while(i >= 1 && (chave < node->chaves[i-1])){
        node->chaves[i] = node->chaves[i-1];
        node->registros[i] = node->registros[i-1];
        i--;
    }
    insereChaveRegistro(node, chave, registro, i);
}

/** ----- Caso 1 ----- */
/** (A) Chave é na folha e a folha tem um minimo de (t/2)-1 chaves, */
static void remocaoCaso1(Node *node, int chave, int idxChave, FILE* arq) {
    if (node == NULL || chave < 0) return;
    
    shift(node, chave, idxChave);
    node->qtdChaves--;
    diskWrite(node, node->ordem, arq);
}

/** ----- Caso 2 ----- */
/* (A) Se o filho a esquerda do nó a remover com chave k tiver pelo menos t/2 elementos, 
* encontra o maior do filho a esquerda (k'), troca de lugar com k e remove k */
static void remocaoCaso2(BT *bt, Node *node, int chave, int idxChave, FILE* arq) {
    if (node == NULL || chave < 0 || idxChave < 0) return;

    if (ehFolhaNode(node) == true) return;

    int ordem = getOrdemNode(node);
    int DIREITA = idxChave+1, ESQUERDA  = idxChave;
    
    Node* esq = diskRead(node->offsetFilhos[ESQUERDA], ordem, arq);
    Node* dir = diskRead(node->offsetFilhos[DIREITA], ordem, arq);

    if (podeRemoverDoNode(node->filhos[ESQUERDA])) {
        int registro_troca = getMaiorRegistro(esq);
        int chave_troca = getMaiorChave(esq);

        trocaConteudos(&registro_troca, &node->registros[idxChave]);
        trocaConteudos(&chave_troca,    &node->chaves[idxChave]);
        setChaveRegistroTroca(esq, chave_troca, registro_troca, getQtdChavesNode(esq)-1);
        /** tira do disco o nó "node" */
        removeNode(bt, esq, node, chave_troca, arq);
        
        /* (B) Se for o filho a direita, a mesma coisa, mas com o menorfilho a direita */
    } else if (podeRemoverDoNode(dir)) {
        int registro_troca = getMenorRegistro(dir);
        int chave_troca = getMenorChave(dir);
        // node->chaves[idxChave] = chave_troca;
        // node->registros[idxChave] = registro_troca;

        trocaConteudos(&registro_troca, &node->registros[idxChave]);
        trocaConteudos(&chave_troca, &node->chaves[idxChave]);
        setChaveRegistroTroca(dir, chave_troca, registro_troca, 0);
        // shift(node->filhos[DIREITA], chave_troca, 0);
        /** tira do disco o nó "node" */
        // node->filhos[DIREITA]->qtdChaves--;
        removeNode(bt, dir, node, chave_troca, arq);
    
    /* (C) Em caso de ambos terem (t/2)-1 chaves, copia as coisa de um nó para para completar o outro */
    } else if (getQtdChavesNode(esq) + getQtdChavesNode(dir) <= ordem-1 ) {
        int registro = node->registros[idxChave];
        shift(node, chave, idxChave);
        Node *old = uneNode(esq, dir, chave, registro);
        shiftFilhos(node, idxChave+1);
        node->qtdChaves--;
        old = liberaNode(old);
        /** tira do disco o nó "node" */
        /** reescreve o node new no lugar do filho a esquerda da chave */
        removeNode(bt, esq, node, chave, arq);
    }
    diskWrite(node, ordem, arq);
    diskWrite(esq, ordem, arq);
    diskWrite(dir, ordem, arq);
}

void removeBT(BT *bt, int chave, FILE *arq) {
    if (bt == NULL || chave < 0) return;

    /** Observações possíveis: Cada nó deve ter pelo menos (t/2) - 1 elementos */
    Node *pai = buscaBT(bt->raiz, NULL, chave, NODE_PAI, arq);
    Node *node = buscaBT(bt->raiz, pai, chave, NODE_CHAVE, arq);

    // não existe nó com a chave requisitada
    if (node == NULL) return;
    removeNode(bt, node, pai, chave, arq);
}

/** ----- Caso 3 ----- */
/**    condicao: x é o nó interno, ci[x] é a subarvore dos filhos de x
 *     (A) (distribuicao) se ci[x] possuir t/2 -1 elementos e possuir um irmao adj com pelo 
 * menos t/2  elementos, move o valor de x para ci[x] e promove uma chave de um dos 
 * irmaos adjacentes
 */
static Node *remocaoCaso3(BT *bt, Node *pai, int chave, int idxChave, FILE* arq) {
    if (pai == NULL || chave < 0 || idxChave < 0) return NULL;
    int ordem = getOrdemNode(pai);

    // encontra o indice do pai q mapeia o filho
    int i = 0, rodou = 0, idxPai = 0;
    for (i = 0; i < getQtdChavesNode(pai) && pai->chaves[i] < chave; i++) { rodou = 1; }
    if (rodou) idxPai = i;

    Node *left, *mid, *right;
    if (idxPai == 0) {
        left = mid = diskRead(pai->offsetFilhos[idxPai], ordem, arq);
        right = diskRead(pai->offsetFilhos[idxPai+1], ordem, arq);

    } else if (idxPai == pai->qtdChaves) { 
        left = diskRead(pai->offsetFilhos[idxPai-1], ordem, arq);
        mid = right = diskRead(pai->offsetFilhos[idxPai], ordem, arq);

    } else {
        left = diskRead(pai->offsetFilhos[idxPai-1], ordem, arq);
        mid = diskRead(pai->offsetFilhos[idxPai], ordem, arq);
        right = diskRead(pai->offsetFilhos[idxPai+1], ordem, arq);
    }

/////////////////////////////
    // pro lado esquerdo
    if (mid != left && getQtdChavesNode(left) >= (ordem - ordem/2)) {
        insereNode(mid, pai->chaves[idxPai-1], pai->registros[idxPai-1]);

        if (!ehFolhaNode(mid)) {
            for(int i = mid->qtdChaves; i >= 0; --i)
                mid->filhos[i+1] = mid->filhos[i];

            mid->filhos[0] = left->filhos[getQtdChavesNode(left)];
        }

        int registro_troca = getMaiorRegistro(left);
        int chave_troca = getMaiorChave(left);
        pai->registros[idxPai-1] = registro_troca;
        pai->chaves[idxPai-1] = chave_troca;
        left->qtdChaves--;
        return mid;

    // pro lado direito
    } else if (mid != right && getQtdChavesNode(right) >= (ordem - ordem/2)) {
        insereNode(mid, pai->chaves[idxPai], pai->registros[idxPai]);

        if (!ehFolhaNode(mid)) {
            mid->filhos[getQtdChavesNode(mid)] = right->filhos[0];
        }

        int registro_troca = getMenorRegistro(right);
        int chave_troca = getMenorChave(right);
        pai->registros[idxPai] = registro_troca;
        pai->chaves[idxPai] = chave_troca;
        shift(right, chave_troca, 0);
        right->qtdChaves--;
        return mid;

    } else {
        if (mid != right) {
            int registroPai = pai->registros[idxPai];
            int chavePai = pai->chaves[idxPai];
            shift(pai, chavePai, idxPai);
            Node *old = uneNode(mid, right, chavePai, registroPai);
            shiftFilhos(pai, idxPai+1);
            pai->qtdChaves--;
            if (pai == bt->raiz && getQtdChavesNode(pai) == 0) {
                bt->raiz = mid;
                pai->ehFolha = true;
                pai = liberaNode(pai);
            } 
            old = liberaNode(old);
            /** tira do disco o nó "node" */
            /** reescreve o node new no lugar do filho a esquerda da chave */
            return mid;

        } else {
            int registroPai = pai->registros[idxPai-1];
            int chavePai = pai->chaves[idxPai-1];
            shift(pai, chavePai, idxPai-1);
            Node *old = uneNode(left, mid, chavePai, registroPai);
            shiftFilhos(pai, idxPai);
            pai->qtdChaves--;
            if (pai == bt->raiz && getQtdChavesNode(pai) == 0) {
                bt->raiz = left;
                pai->ehFolha = true;
                pai = liberaNode(pai);
            }
            old = liberaNode(old);
            /** tira do disco o nó "node" */
            /** reescreve o node new no lugar do filho a esquerda da chave */
            return left;
        }
    }
}

void removeNode(BT *bt, Node *node, Node *pai, int chave, FILE* arq) {
    if (node == NULL || chave < 0) return;

    int idxChave = getIdxChave(node, chave);
    if (podeRemoverDoNode(node) == false && pai) {
        node = remocaoCaso3(bt, pai, chave, idxChave, arq);
        idxChave = getIdxChave(node, chave);
    }

    if (ehFolhaNode(node)) {
        remocaoCaso1(node, chave, idxChave, arq);
    } else {
        remocaoCaso2(bt, node, chave, idxChave, arq);
    }
}

Node *buscaBT(Node *node, Node *pai, int chave, int MODO_BUSCA, FILE *arq) {
    if(node == NULL) return NULL;

    int i = 0;
    while(i < node->qtdChaves && chave > node->chaves[i]) i++;
    
    switch (MODO_BUSCA) {
        case NODE_CHAVE:
            if(i < node->qtdChaves && chave == node->chaves[i]) return node;
            else if(ehFolhaNode(node)) return NULL;
            else{
                //diskRead(node->filhos[i]);
                return buscaBT(node->filhos[i], node, chave, NODE_CHAVE, arq);
            }

        case NODE_PAI:
            if(i < node->qtdChaves && chave == node->chaves[i]) return pai;
            else if(ehFolhaNode(node)) return NULL;
            else{
                //diskRead(node->filhos[i]);
                return buscaBT(node->filhos[i], node, chave, NODE_PAI, arq);
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

    bt->raiz = liberaNode(bt->raiz);
    free(bt);
}

void printBT(BT* bt, FILE* arq, FILE *bin){
    if(bt == NULL) return;

    if (arq) {
        fprintf(arq, "\n-- ARVORE B\n");
        //le bin

        Fila* fila = criaFila();
        insereFila(fila, bt->raiz);

        int n = 1; //numero de nodes na mesma linha de impressao
        while (!filaVazia(fila)) {
            int numNodesLinha = 0; //numero de nodes na mesma linha de impressao
            for(int j = 0; j < n ; j++) {
                Node* node = (Node*) removeFila(fila);
                if (node) printNode(node, arq);

                if(!ehFolhaNode(node)) {
                    int tamanho = node->qtdChaves;
                    for(int i=0; i <= tamanho; i++) {
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
}

void diskWrite(Node *node, int order, FILE *fp)
{
    int offsetFile = calculaOffset(getOffset(node), order);
    int inicio = offsetFile*(getOffset(node)-1);
    fseek(fp, offsetFile, 0);                                 

    int numKeys = getQtdChavesNode(node);
    bool isLeaf = ehFolhaNode(node);
    int posInDisk = getOffset(node);
    

    fwrite(&numKeys, sizeof(getQtdChavesNode(node)), 1, fp);          // write the information to the file
    fwrite(&isLeaf, sizeof(ehFolhaNode(node)), 1, fp);
    fwrite(&posInDisk, sizeof(getOffset(node)), 1, fp);
    
    for(int i = 0;i<node->qtdChaves;i++){
        fwrite(&node->chaves[i], sizeof(int), 1, fp);
    }
    for(int i = 0;i<node->qtdChaves;i++){
        fwrite(&node->registros[i], sizeof(int), 1, fp);
    }
    if(!node->ehFolha){
        for(int i = 0;i<node->qtdChaves+1;i++){
            fwrite(&node->offsetFilhos[i], sizeof(int), 1, fp);
        }
    }
    

}

Node *diskRead(int offset, int ordem, FILE *fp)
{
    int offsetFile = calculaOffset(offset, ordem);
    fseek(fp, offsetFile,0);      // set the file pointer there

    int numKeys = 0, pos_in_disk = 0, key = 0;
    bool isLeaf;

    fread(&numKeys, sizeof(int), 1, fp);    // read the information from the file
    fread(&isLeaf, sizeof(bool), 1, fp);
    fread(&pos_in_disk, sizeof(int), 1, fp);

    Node *read_node = criaNode( ordem-1, isLeaf,offset);
    read_node->qtdChaves = numKeys;
    for(int i = 0;i<read_node->qtdChaves;i++){
        fread(&read_node->chaves[i], sizeof(int), 1, fp);
    }
    for(int i = 0;i<read_node->qtdChaves;i++){
        fread(&read_node->registros[i], sizeof(int), 1, fp);
    }
    if(!read_node->ehFolha){
        for(int i = 0;i<read_node->qtdChaves+1;i++){
            fread(&read_node->offsetFilhos[i], sizeof(int), 1, fp);
        }
        for(int i=0; i <read_node->qtdChaves+1 ; i++){
            read_node->filhos[i] = diskRead(read_node->offsetFilhos[i],ordem,fp);
    }
    }
    
    
    return read_node;
}