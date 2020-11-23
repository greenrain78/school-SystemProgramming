#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>
#include <unistd.h>

#define NAMEC "/conv_message"
#define NAMEP "/pool_massage"

void makeMatrix(int ** matrixm, int X, int Y);
void checkNum(int argc, char** argv, int* x, int* y);
void Convlayer(int x, int y, int** sample1, int** sample2);
void ConvChild(mqd_t mqdes, size_t msg_size);
void freeMatrix(int x, int** sample1, int** sample2);

int main(int argc, char**argv)
{
    int X = 0;
    int Y = 0;
    int ** sample1;
    int ** sample2;

    //check num
    checkNum(argc, argv, &X, &Y);

    sample1 = (int**)malloc(sizeof(int*)*X);
    for (size_t i = 0; i < X; i++)
    {
        sample1[i] = (int*)malloc(sizeof(int)*Y);
    }
    sample2 = (int**)malloc(sizeof(int*)*(X-2));
    for (size_t i = 0; i < X-2; i++)
    {
        sample2[i] = (int*)malloc(sizeof(int)*(Y-2));
    }
    
    Convlayer(X, Y, sample1, sample2);

    //make matrix

    //check matrix
    printf("\n sample1\n");
    for (size_t i = 0; i < X; i++)
    {
        for (size_t j = 0; j < Y; j++)
        {
            printf("%d ", sample1[i][j]);
        }
        printf("\n");
    }
    printf("\n sample2\n");
    for (size_t i = 0; i < X-2; i++)
    {
        for (size_t j = 0; j < Y-2; j++)
        {
            printf("%d ", sample2[i][j]);
        }
        printf("\n");
    }


    return 0;
}
void checkNum(int argc, char** argv, int* x, int* y){
    if ( argc == 2)
    {
        int tmp = atoi(argv[1]);
        if(tmp%2==0&&tmp>=4){
            (*x) = tmp;
            (*y) = tmp;
        }
    }
    else
    {
        printf("input wrong number\n");
        exit(0);
    }
    printf("X : %d, Y : %d\n", (*x), (*y));
}
void Convlayer(int x, int y, int** sample1, int** sample2){
    struct mq_attr attr;
    mqd_t mqdes;
    size_t msg_size = sizeof(int)*9;
    int id = 0;

   //setting message queue
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = msg_size;

    //open message queue
    mqdes = mq_open(NAMEC, O_CREAT|O_RDWR, 0600, &attr);
    if (mqdes < 0) {
        perror("mq_open()");
        exit(0);
    }
    printf("make child \n");
    //make child
    for (size_t i = 0; i < x-2; i++)
    {
        for (size_t j = 0; j < y-2; j++)
        {
            printf("make child:  %ld %ld\n", (i+1), (j+1));
            if(fork() == 0){
                ConvChild(mqdes, msg_size);
                freeMatrix(x, sample1, sample2);
                mq_close(mqdes);
                exit(0);
            }
        }    
    }    
    printf("end child\n");
    //make sample
    makeMatrix(sample1, x, y);

    printf("\n sample1\n");
    for (size_t i = 0; i < x; i++)
    {
        for (size_t j = 0; j < y; j++)
        {
            printf("%d ", sample1[i][j]);
        }
        printf("\n");
    }

    //cut matrix&send
    for (size_t i = 0; i < x-2; i++)
    {
        for (size_t j = 0; j < y-2; j++)
        {
            int tmp_send[9]= {sample1[i][j], sample1[i][j+1], sample1[i][j+2], sample1[i+1][j], sample1[i+1][j+1], sample1[i+1][j+2], sample1[i+2][j], sample1[i+2][j+1], sample1[i+2][j+2]};
            printf("send %ld , %ld\n", i, j);
            for (size_t k = 0; k < 9; k++)
            {
                printf("%d ", tmp_send[k]);
            }
            printf("\n");
            
            if (mq_send(mqdes, (char*)tmp_send, msg_size, (i+1)*(j+1)) == -1){
                perror("mq_send()");
                freeMatrix(x, sample1, sample2);
                mq_close(mqdes);
                mq_unlink(NAMEC);
                break;
            }
        }
    }
    //receive
    for (size_t i = 0; i < (x-2)*(y-2); i++)
    {
        printf("receive %ld", i);
        int tmp[3][3];
        unsigned int prio;

        if (mq_receive(mqdes, (char*)tmp, msg_size, &prio) == -1){
            perror("ConvChild()");
            freeMatrix(x, sample1, sample2);
            mq_close(mqdes);
            mq_unlink(NAMEC);
            exit(0);
        }
        else
        {
            int idx1 = prio/(x-2);
            int idx2 = prio%(x-2);
            sample2[idx1][idx2] = tmp[0][0];
        }
    }
}
void ConvChild(mqd_t mqdes, size_t msg_size){
    //make 3X3
    int origin[9];
    int filter[9] = {-1, -1, -1, -1, 8, -1, -1, -1, -1};
    int sum[9];
    unsigned int prio;

    
    //receive 
    if (mq_receive(mqdes, (char*)origin, msg_size, &prio) == -1){
        perror("ConvChild()");
        exit(0);
    }
    
    printf("\n receive %d\n", origin[0]);

    for (size_t i = 0; i < 9; i++)
    {
        printf("%d ", origin[i]);
    }
    //calcul
    for (size_t i = 0; i < 9; i++)
    {
        sum[0] += origin[i]*filter[i];
    }
    //send
    if (mq_send(mqdes, (char*)sum, msg_size, prio) == -1){
        perror("mq_send()");
        exit(0);
    }
}
void freeMatrix(int x, int** sample1, int** sample2){
    for (size_t i = 0; i < x; i++)
    {
        free(sample1[i]);
    }
    free(sample1);
    for (size_t i = 0; i < x-2; i++)
    {
        free(sample1[i]);
    }
    free(sample1);
}