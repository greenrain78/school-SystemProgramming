#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define NAME "/testMatrix"
void makeMatrix(int ** matrixm, int X, int Y);



int main(void)
{
        
    struct mq_attr attr;
    mqd_t mqdes;

    int sizeMatrix;

    attr.mq_maxmsg = 20;
    attr.mq_msgsize = 20;
    

    //open message
    mqdes = mq_open(NAME, O_CREAT | O_WRONLY, 0600, &attr);
    if (mqdes < 0){
        perror("mq_open()");
        exit(0);
    }
    //send message
    printf("now1\n");
    printf("%ld\n", attr.mq_msgsize);
    mqd_t tmp = mq_send(mqdes, (char*)"aaaaaqqqq", 11, 1);
    if ( tmp<0){
            perror("mq_send()");
        }else
        {
            printf(" clear all send\n");        
        }
    mq_close(mqdes);

    return 0;
}