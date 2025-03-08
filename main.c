#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[]){
    if(argc < 3){
        printf("Faltam parâmetros para rodar o código\n");
        return 1;
    }
    char in[100], out[100];
    sprintf(in, "%s", argv[1]);
    sprintf(out, "%s", argv[2]);

    FILE* fIn = fopen(in, "r");
    FILE* fOut = fopen(out, "w");

    int ordemBT;
    fscanf(fIn, "%d%*c", &ordemBT);

    //cria BT

    char c;
    int n, chave, registro = 0;
    fscanf(fIn, "%d%*c", &n);
    for(int i=0; i<n; i++){
        fscanf(fIn, "%c ", &c);
        if(c == 'I'){
            fscanf(fIn, "%d, %d%*c", &chave, &registro);
            //insere na BT
        }
        else if(c == 'R'){
            fscanf(fIn, "%d%*c", &chave);
            //remove da BT
        }
        else if(c == 'B'){
            fscanf(fIn, "%d%*c", &chave);
            //busca na BT
        }
    }

    //libera BT
    fclose(fIn);
    fclose(fOut);

    return 0;
}