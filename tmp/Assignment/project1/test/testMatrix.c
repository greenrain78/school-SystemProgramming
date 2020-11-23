#include <stdio.h>
#include <stdlib.h>

void makeMatrix(int ** matrixm, int X, int Y);

int main(int atgc, char** argv){

    int X = 3;
    int Y = 2;

    int** sample1;
    sample1 = (int**) malloc(sizeof(int*)*X);
    for(int i=0;i<X;i++){
        sample1[i] = (int*)malloc(sizeof(int)*Y);
    }

    makeMatrix(sample1, X, Y);

    for(int i=0; i<X;i++){
        for(int j=0; j<Y;j++){
            printf("%d ", sample1[i][j]);
        }
    }
    printf("\n");

    for(int i=0; i<X;i++){
        free(sample1[i]);
    }
    free(sample1);
}