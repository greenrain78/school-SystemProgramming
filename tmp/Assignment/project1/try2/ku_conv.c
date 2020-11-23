#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NAMEC "/conv_message"
#define NAMEP "/pool_massage"
#define CONV_SIZE 40
#define POOL_SIZE 20

void makeMatrix(int ** matrixm, int X, int Y);
void checkNum(int argc, char** argv, int* x, int* y);

void Convlayer(int x, int y, int** sample1, int** sample2);
mqd_t ConvOpenMessage();
int ConvMakeChild(int x, int y);
void ConvSend(mqd_t mqdes, int* message, unsigned int prio);
void ConvReceive(mqd_t mqdes, int* message, unsigned int* prio);
void ConvCalcul(int* message);
void ConvKill(int x, int y, int** sample1, int** sample2);

void PoolLayer(int x, int y, int** sample2, int** sample3);
mqd_t PoolOpenMessage();
int PoolMakeChild(int x, int y);
void PoolSend(mqd_t mqdes, int* message, unsigned int prio);
void PoolReceive(mqd_t mqdes, int* message, unsigned int* prio);
void PoolCalcul(int* message);
void PoolKill(int x, int y, int** sample1, int** sample2);

int main(int argc, char**argv)
{
    int X = 0;
    int Y = 0;
    int ** sample1;
    int ** sample2;
    int ** sample3;

    //check num
    checkNum(argc, argv, &X, &Y);
    //make sample
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
    //make matrix
    makeMatrix(sample1, X, Y);
    //Convolution Layer
    Convlayer(X, Y, sample1, sample2);
    printf("---------end Conv --------\n");
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
    //setting
    for (size_t i = 0; i < X; i++)
    {
        free(sample1[i]);
    }
    free(sample1);
    sample3 = (int**)malloc(sizeof(int*)*((X-2)/2));
    for (size_t i = 0; i < (X-2)/2; i++)
    {
        sample3[i] = (int*)malloc(sizeof(int)*((Y-2)/2));
    }
    //Max Polling Layer
    PoolLayer(X-2, Y-2, sample2, sample3);
    for (size_t i = 0; i < X-2; i++)
    {
        free(sample2[i]);
    }
    free(sample2);

    //check matrix

    printf("\n sample3\n");
    for (size_t i = 0; i < (X-2)/2; i++)
    {
        for (size_t j = 0; j < (Y-2)/2; j++)
        {
            printf("%d ", sample3[i][j]);
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
    mqd_t mqdes;
    int id = 0;
    //open message queue
    printf("open message queue \n");
    mqdes = ConvOpenMessage();

    //make child
    printf("make child \n");
    id = ConvMakeChild(x, y);

    //proformance
    if (id == 0)
    {   //child
        int message[10];
        unsigned int prio;
        ConvReceive(mqdes, message, &prio);                
        printf("receive : %d\t", message[9]);
        for (size_t k = 0; k < 9; k++){
            printf("%d ", message[k]);
        }
        ConvCalcul(message);
        ConvSend(mqdes, message, message[9]);
        printf("end child %d\n", id);
        ConvKill(x, y, sample1, sample2);
        exit(0);
    }else
    {   //parent
        int child_status;
        int message[10];
        int pos = 0;

        //cut matrix&send
        for (size_t i = 0; i < x-2; i++){
            for (size_t j = 0; j < y-2; j++){
                int message[10]= {sample1[i][j], sample1[i][j+1], sample1[i][j+2], sample1[i+1][j], sample1[i+1][j+1], sample1[i+1][j+2], sample1[i+2][j], sample1[i+2][j+1], sample1[i+2][j+2], pos++};
                printf("send %ld , %ld\n", i, j);
                for (size_t k = 0; k < 9; k++){
                    printf("%d ", message[k]);
                }
                printf("\npos : %d\n", pos);
                ConvSend(mqdes, message, 0);
            }
        }
        unsigned int prio;
        //wait & receive
        for (size_t i = 0; i < id; i++)
        {
            wait(&child_status);
            if (WIFEXITED(child_status))
            {   //correct
                ConvReceive(mqdes, message, &prio);
                sample2[message[9]/(x-2)][message[9]%(y-2)] = message[0];
            }
            else
            {   //error
                printf("child return error\n");
            }
        }
    }
    mq_close(mqdes);
    mq_unlink(NAMEC);
 
}
mqd_t ConvOpenMessage(){
    struct mq_attr attr;
    mqd_t mqdes;

   //setting message queue
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = CONV_SIZE;

    //open message queue
    mqdes = mq_open(NAMEC, O_CREAT|O_RDWR, 0600, &attr);
    if (mqdes < 0) {
        perror("mq_open()");
        exit(0);

    }

    return mqdes;
}
int ConvMakeChild(int x, int y){
    for (size_t i = 0; i < x-2; i++)
    {
        for (size_t j = 0; j < y-2; j++)
        {
            printf("make child:  %ld %ld\n", (i+1), (j+1));
            if(fork() == 0){
                return 0;
            }
        }    
    }    
    printf("end make child\n");
    return (x-2)*(y-2);
}
void ConvSend(mqd_t mqdes, int* message, unsigned int prio){
    if (mq_send(mqdes, (char*)message, CONV_SIZE, prio) == -1){
                perror("mq_send()");
                exit(0);
            }
}
void ConvReceive(mqd_t mqdes, int* message, unsigned int* prio){
    if (mq_receive(mqdes, (char*)message, CONV_SIZE, prio) == -1){
            perror("mq_receive()");
            exit(0);
        }
}
void ConvCalcul(int* message){
    int sum = 0;
    int filter[9] = {-1, -1, -1, -1, 8, -1, -1, -1, -1};
    for (size_t i = 0; i < 9; i++)
    {
        sum+=message[i]*filter[i];
    }
    message[0] = sum;
}
void ConvKill(int x, int y, int** sample1, int** sample2){
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

void PoolLayer(int x, int y, int** sample2, int** sample3){
    mqd_t mqdes;
    int id = 0;
    //open message queue
    printf("open message queue %d %d \n", x, y);
    mqdes = PoolOpenMessage();

    //make child
    printf("make child \n");
    id = PoolMakeChild(x, y);

    printf("proformance start \n\n");
    //proformance
    if (id == 0)
    {   //child
        int message[5];
        unsigned int prio;
        PoolReceive(mqdes, message, &prio);                
        printf("receive : %d\t", message[4]);
        for (size_t k = 0; k < 5; k++){
            printf("%d ", message[k]);
        }
        PoolCalcul(message);
        PoolSend(mqdes, message, message[4]);
        printf("end child %d\n", id);
        // PoolKill(x, y, sample2, sample3);
        exit(0);
    }else
    {   //parent
        int child_status;
        int message[5];
        int pos = 0;

        //cut matrix&send
        for (size_t i = 0; i < x/2; i++){
            for (size_t j = 0; j < y/2; j++){
                int message[10]= {sample2[i][j], sample2[i][j+1], sample2[i+1][j], sample2[i+1][j+1], pos++};
                printf("send %ld , %ld\n", i, j);
                for (size_t k = 0; k < 5; k++){
                    printf("%d ", message[k]);
                }
                printf("\npos : %d\n", pos);
                PoolSend(mqdes, message, 0);
            }
        }
        unsigned int prio;
        //wait & receive
        for (size_t i = 0; i < id; i++)
        {
            wait(&child_status);
            if (WIFEXITED(child_status))
            {   //correct
                PoolReceive(mqdes, message, &prio);
                sample3[message[5]/(x/2)][message[5]%(y/2)] = message[0];
            }
            else
            {   //error
                printf("child return error\n");
            }
        }
    }
    mq_close(mqdes);
    mq_unlink(NAMEP);
 
}
mqd_t PoolOpenMessage(){
    struct mq_attr attr;
    mqd_t mqdes;

   //setting message queue
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = POOL_SIZE;

    //open message queue
    mqdes = mq_open(NAMEP, O_CREAT|O_RDWR, 0600, &attr);
    if (mqdes < 0) {
        perror("mq_open()");
        exit(0);

    }

    return mqdes;
}
int PoolMakeChild(int x, int y){
    for (size_t i = 0; i < x/2; i++)
    {
        for (size_t j = 0; j < y/2; j++)
        {
            printf("make child:  %ld %ld\n", (i+1), (j+1));
            if(fork() == 0){
                return 0;
            }
        }    
    }    
    printf("end make child\n");
    return (x/2)*(y/2);
}
void PoolSend(mqd_t mqdes, int* message, unsigned int prio){
    if (mq_send(mqdes, (char*)message, POOL_SIZE, prio) == -1){
                perror("mq_send()");
                exit(0);
            }
}
void PoolReceive(mqd_t mqdes, int* message, unsigned int* prio){
    if (mq_receive(mqdes, (char*)message, POOL_SIZE, prio) == -1){
            perror("mq_receive()");
            exit(0);
        }
}
void PoolCalcul(int* message){
    int max = message[0];
    for (size_t i = 1; i < 4; i++)
    {
        if (max < message[i])
        {
            max = message[i];
        }
    }
    message[0] = max;
}
