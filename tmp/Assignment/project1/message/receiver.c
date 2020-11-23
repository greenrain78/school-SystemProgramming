#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>

#define MSG_SIZE 100
#define NAME "/testmassage"

int main(void)
{
    struct mq_attr attr;
    mqd_t mqdes;
    int value = 0;
    unsigned int prio;
    int count = 7;
    int N = 4;
    int * sample1 = (int*)malloc(sizeof(int)*N);

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSG_SIZE;

    mqdes = mq_open(NAME, O_CREAT|O_RDWR, 0600, &attr);

    if (mqdes < 0) {
        perror("mq_open()");
        exit(0);
    }

    for (size_t i = 0; i < count; i++)
    {
        printf("%ld\n", i);
        if (mq_receive(mqdes, (char*)sample1, MSG_SIZE, 0) == -1){
            perror("mq_receive()");
            break;
        }
        for (size_t j = 0; j < N; j++)
        {
            printf("%d ", sample1[j]);
        }
        printf("\n");
    }
    mq_close(mqdes);
    mq_unlink(NAME);

    return 0;
}
