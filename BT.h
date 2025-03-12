#ifndef _BT_H_
#define _BT_H_

typedef struct BT BT;
typedef struct Node Node;

BT *criaBT(int ordem);

void insereBT(BT *bt, int chave, int registro);

Node *removeBT(BT *bt, int chave);

bool buscaBT(Node *node, int chave);

void printBT(BT* bt, FILE* arq);

void liberaBT(BT *bt);

Node *getRaizBT(BT* bt);
    
#endif // !_BT_H_