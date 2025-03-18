#include "fila.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct cel Cel;

struct fila{
    Cel* prim, *ult;
};

struct cel{
    void* item;
    Cel* prox;
};

Fila* criaFila(){
    Fila* fila = malloc(sizeof(Fila));
    fila->prim = fila->ult = NULL;
    return fila;
}

void insereFila(Fila* fila, void* item){
    if (item == NULL) return;

    Cel* c = malloc(sizeof(Cel));
    c->item = item;
    c->prox = NULL;

    if(fila->prim == NULL) fila->prim = c;

    if(fila->ult) fila->ult->prox = c;
    fila->ult = c;
}

void* removeFila(Fila* fila) {
    if (fila == NULL || fila->prim == NULL) return NULL;

    Cel* c = fila->prim;
    if(fila->prim){
        fila->prim = fila->prim->prox;
        if (!fila->prim) fila->ult = NULL;
        void* item = c->item;
        free(c);
        return item;
    }
    return NULL;
}

void liberaFila(Fila* fila){
    if (fila == NULL) return;

    Cel* c, *aux = fila->prim;
    while(aux){
        c = aux;
        aux = aux->prox;
        free(c);
    }
    free(fila);
}

int filaVazia(Fila* fila){
    if (fila == NULL) return 0;

    if(fila->prim) return 0;
    return 1;
}