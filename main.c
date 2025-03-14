#include <stdio.h>
#include <stdlib.h>
#include "BT.h"
#include "fila.h"

int main(int argc, char const *argv[]){
    /*if(argc < 3){
        printf("Faltam parâmetros para rodar o código\n");
        return 1;
    }
    char in[100], out[100];
    sprintf(in, "%s", argv[1]);
    sprintf(out, "%s", argv[2]);
*/
    FILE* fIn = fopen("caso_teste_5.txt", "r");
    if(fIn == NULL){
        printf("Erro na criação do arquivo de entrada\n");
        return 1;
    }

    FILE* fOut = fopen("saida.txt", "w");
    if(fOut == NULL){
        printf("Erro na criação do arquivo de saída\n");
        return 1;
    }

    int ordemBT, nOperacoes;
    fscanf(fIn, "%d%*c", &ordemBT);
    fscanf(fIn, "%d%*c", &nOperacoes);

    BT* bt = criaBT(ordemBT);
    
    char c;
    int chave, registro = 0;
    for(int i=0; i<nOperacoes; i++){
        fscanf(fIn, "%c ", &c);

        if(c == 'I'){
            fscanf(fIn, "%d, %d%*c", &chave, &registro);
            insereBT(bt, chave, registro);

        } else if(c == 'R'){
            fscanf(fIn, "%d%*c", &chave);
            removeBT(bt, getRaizBT(bt), chave);

        } else if(c == 'B'){
            fscanf(fIn, "%d%*c", &chave);
            if(buscaBT(getRaizBT(bt), chave)) fprintf(fOut, "O REGISTRO ESTA NA ARVORE!\n");
            else fprintf(fOut, "O REGISTRO NAO ESTA NA ARVORE!\n");
        }
    }

    printBT(bt, fOut);

    liberaBT(bt);
    fclose(fIn);
    fclose(fOut);

    return 0;
}