#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>

#define NAME "/testMatrix"

int main(void)
{
        
    struct mq_attr attr;
    mqd_t mqdes;
    int ** sample1;
    int X = 4;
    int Y = 3;
    int sizeMatrix;

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 10;

    //open message
    mqdes = mq_open(NAME, O_CREAT | O_WRONLY, 0600, &attr);
    if (mqdes < 0){
        perror("mq_open()");
        exit(0);
    }
    
    for(int i=0; i<X;i++){
        if (mq_receive(mqdes, (char*)sample1[i], sizeMatrix, 0) ==-1){
            perror("mq_receive()");
            break;
        }
        else{
            printf("clear send message: ");
            for(int j=0;j<Y;j++){
                printf("%d ", sample1[i][j]);
            }
            printf("\n");
        }
    }
    printf("clear all receive");
    mq_close(mqdes);
    mq_unlink(NAME);
    return 0;
}