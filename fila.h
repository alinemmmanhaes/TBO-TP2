#if !defined(FILA_H)
#define FILA_H

typedef struct fila Fila;

Fila* criaFila();

void insereFila(Fila* fila, void* item);

void* removeFila(Fila* fila);

void liberaFila(Fila* fila);

int filaVazia(Fila* fila);

#endif // FILA_H
