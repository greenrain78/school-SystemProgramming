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

    mqdes = mq_open(NAME, O_CREAT|O_WRONLY, 0600, &attr);

    if (mqdes < 0) {
        perror("mq_open()");
        exit(0);
    }

    for (size_t i = 0; i < count; i++)
    {
        for (size_t j = 0; j < N; j++)
        {
            sample1[j] = value++;
        }
        if (mq_send(mqdes, (char*)sample1, MSG_SIZE, 0) == -1){
            perror("mq_send()");
            break;
        }else
        {
            printf("Sending a message (val: %d)\n", value);
        }
    }

    mq_close(mqdes);
    return 0;
}