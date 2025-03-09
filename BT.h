#ifndef _BT_H_
#define _BT_H_

#include "Node.h"

typedef struct BT BT;

BT *criaBT();

void insereBT(BT *bt, int chave, int registro);

Node *removeBT(BT *bt, int chave);

Node *buscaBT(BT *bt, int chave);

void liberaBT(BT *bt);
    

#endif // !_BT_H_
