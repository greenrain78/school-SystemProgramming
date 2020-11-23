#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>
#include <unistd.h>

#define NAME "/sendMatrix"
#define CONV_SIZE 36

void ConvSend(mqd_t mqdes, int* message, unsigned int prio);
void ConvReceive(mqd_t mqdes, int* message, unsigned int* prio);

int main(){
    struct mq_attr attr;
    mqd_t mqdes;
    int message[9];
    unsigned int prio = 0;

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = CONV_SIZE;
    //open
    mqdes = mq_open(NAME, O_CREAT|O_RDWR, 0600, &attr);
    for (size_t i = 0; i < 4; i++)
    {
        if (fork() == 0)
        {
            ConvReceive(mqdes, message, &prio);
            printf("end child %ld\n", i);
            for (size_t k = 0; k < 9; k++){
            printf("%d ", message[k]);
            }
            exit(0);
        }
    }
    //send
    for (size_t k = 0; k < 9; k++){
        message[k] = k;
    }
    
    for (size_t i = 0; i < 4; i++)
    {
        printf("send %ld", i);
        ConvSend(mqdes, message, i);
    }
    
    mq_close(mqdes);
    mq_unlink(NAME);

}
void ConvSend(mqd_t mqdes, int* message, unsigned int prio){
    if (mq_send(mqdes, (char*)message, CONV_SIZE, prio) == -1){
                perror("mq_send()");
                exit(0);
            }
}
void ConvReceive(mqd_t mqdes, int* message, unsigned int* prio){
      if (mq_receive(mqdes, (char*)message, CONV_SIZE, prio) == -1){
            perror("ConvChild()");
            exit(0);
        }
}