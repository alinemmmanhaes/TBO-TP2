#ifndef _NODE_H_
#define _NODE_H_

#include <stdbool.h>

typedef struct Node Node;

Node *criaNode();

void liberaNode(Node *node);

int getNumElementosNode(Node *node);

void* getChavesNode(Node *node);

bool ehFolhaNode(Node *node);

#endif