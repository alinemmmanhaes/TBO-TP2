#ifndef _BT_H_
#define _BT_H_


typedef struct BT BT;
typedef struct Node Node;

#define NODE_CHAVE 0
#define NODE_PAI 1

BT *criaBT(int ordem);

void insereBT(BT *bt, int chave, int registro);

void removeBT(BT *bt, int chave);

Node *buscaBT(Node *node, Node *pai, int chave, int MODO_BUSCA);

void printBT(BT* bt, FILE* arq);

void insereChaveRegistro(Node *n, int chave, int registro, int ind);

Node *getRaizBT(BT* bt);

int getOrdemBT(BT *bt);

int getNumNosBT(BT *bt);

void liberaBT(BT *bt);

#endif // !_BT_H_