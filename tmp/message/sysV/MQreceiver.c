#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
# define MAX_ID 5
int main (void)
{
    key_t ipckey;
    int mqdes, i;
    size_t buf_len;
    struct {
        long id;
        int value;
    } mymsg;

    buf_len = sizeof(mymsg.value);
    ipckey = ftok("./tmp/foo", 1946);
    mqdes = msgget(ipckey, IPC_CREAT|0600);
    if( mqdes < 0 ){
        perror("msgget()");
        exit(0);
    }
    for( i = 0 ; i <= MAX_ID ; i++ ){
        if( msgrcv(mqdes, &mymsg, buf_len, i+1, 0) == -1 ){
            perror("msgrcv()");
            exit(0);
    }
    else
        printf("Received a message (val: %d, id: %ld)\n", mymsg.value, mymsg.id);
    }
    return 0;
}