#ifndef _BT_H_
#define _BT_H_

typedef struct BT BT;
typedef struct Node Node;

BT *criaBT(int ordem);

void insereBT(BT *bt, int chave, int registro);

Node *removeBT(BT *bt, int chave);

Node *buscaBT(Node *node, int chave);

void printBT(BT* bt, FILE* arq);

Node *getRaizBT(BT* bt);

int getOrdem(BT *bt);

int getNumNos(BT *bt);

void liberaBT(BT *bt);

#endif // !_BT_H_