#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>
#include <unistd.h>

#define NAME "/sendMatrix"

void send(mqd_t mqdes, size_t msg_size);
void receive(mqd_t mqdes, size_t msg_size, int id);

int main(){
    struct mq_attr attr;
    mqd_t mqdes;
    size_t msg_size = sizeof(int)*9;
    

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = msg_size;
    //open
    mqdes = mq_open(NAME, O_CREAT|O_RDWR, 0600, &attr);
    for (size_t i = 0; i < 4; i++)
    {
        if (fork() == 0)
        {
            receive(mqdes,msg_size, i);
            printf("end child %ld\n", i);
            exit(0);
        }
    }
    for (size_t i = 0; i < 4; i++)
    {
        printf("send %ld", i);
        send(mqdes, msg_size);
    }
    
    mq_close(mqdes);
    mq_unlink(NAME);

}
void send(mqd_t mqdes, size_t msg_size){
    int sample[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    if (mq_send(mqdes, (char*)sample, msg_size, 0) == -1){
        perror("mq_send()");
    }

}
void receive(mqd_t mqdes, size_t msg_size, int id){
        int sample[9];
        if (mq_receive(mqdes, (char*)sample, msg_size, NULL) == -1){
            perror("ConvChild()");
            mq_close(mqdes);
            exit(0);
        }
        for (size_t i = 0; i < 9; i++)
        {
            sample[i] += id;
            printf("%d ", sample[i]);
        }
        printf("\n");
}