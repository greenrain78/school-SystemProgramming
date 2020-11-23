#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>

#define NAME "/testmassage"

int main(int argc, char**argv)
{
    struct mq_attr attr;
    mqd_t mqdes;
    int X = 3;
    int Y = 2;

    //check num
    if ( argc == 2)
    {
        int tmp = atoi(argv[1]);
        if(tmp%2==0&&tmp>=4){
            X = tmp;
            Y = tmp;
        }
    }
    printf("X : %d, Y : %d", X, Y);
    size_t msg_size = sizeof(int)*Y;

    int ** sample1;
    sample1 = (int**)malloc(sizeof(int*)*X);
    for (size_t i = 0; i < X; i++)
    {
        sample1[i] = (int*)malloc(sizeof(int)*Y);
    }

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = msg_size;

    mqdes = mq_open(NAME, O_CREAT|O_RDWR, 0600, &attr);
    if (mqdes < 0) {
        perror("mq_open()");
        exit(0);
    }

        for (size_t i = 0; i < X; i++)
    {
        if (mq_receive(mqdes, (char*)sample1[i], msg_size, 0) == -1){
            perror("mq_receive()");
            break;
        }else
        {
            printf("mq_receive a message : %ld)\n", i);
            for (size_t j = 0; j < Y; j++)
            {
                printf("%d ", sample1[i][j]);
            }
            printf("\n");
        }
    }
    for (size_t i = 0; i < X; i++)
    {
        free(sample1[i]);
    }
    free(sample1);
    mq_close(mqdes);
    mq_unlink(NAME);

    return 0;
}
