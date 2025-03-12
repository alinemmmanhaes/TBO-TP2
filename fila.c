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
    Cel* c = malloc(sizeof(Cel));
    c->item = item;
    c->prox = NULL;

    if(fila->prim == NULL) fila->prim = c;

    if(fila->ult) fila->ult->prox = c;
    fila->ult = c;
}

void* removeFila(Fila* fila){
    Cel* c = fila->prim;

    if(fila->prim){
        fila->prim = fila->prim->prox;
        void* item = c->item;
        free(c);
        return item;
    }
    return NULL;
}

void liberaFila(Fila* fila){
    Cel* c, *aux = fila->prim;
    while(aux){
        c = aux;
        aux = aux->prox;
        free(c);
    }
    free(fila);
}

int filaVazia(Fila* fila){
    if(fila->prim) return 0;
    return 1;
}