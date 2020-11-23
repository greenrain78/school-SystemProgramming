#include <stdio.h>
#include <stdlib.h>
struct msg_matrix_3x3_st{
    long id;
    int value[3][3];
};

struct msg_matrix_2x2_st{
    long id;
    int value[2][2];
};

struct msg_ret_st{
    long id;
    int value;
};

void signalhandler(){
}

mqd_t ku_mq_open(char *mqname, int msg_size){
    struct mq_attr attr;
    mqd_t mq_des;
    attr.mq_maxmsg;
    attr.mq_msgsize = msg_size;
    mq_des = mq_open(mqname, O_CREAT|O_RDWR, 0600, &attr);
    if(mq_des <0){
        perror("mq_open()");
        exit(0);
    }
    return mq_des;
}
id ku_mq_send(char* mqname, char* data, int msg_size, unsigned int id){
    mqd_t mq_des = ku_mq_open(mqname, msg_size);
    if(mq_send(mq_des, data, msg_size, id) == -1){
        perror("mq_send");
        exit(0);
    }
    mq_close(mq_des);
}
void ku_mq_receive(char* mqname, char* data, int msg_size, unsigned int id){
    unsigned int msgid;
    mqd_t mq_des = 
}
//작성하다가 귀찮
