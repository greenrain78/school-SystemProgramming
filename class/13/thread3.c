#include <stdio.h>

volatile int ncount;

void *do_loop(void *loop);

int main(int argc, char *argv[])
{
    int status, sec;
    pthread_t thread_id;
    ncount = 0;
    sec = atoi(argv[1]);
    status = pthread_create(&thread_id, NULL, do_loop, NULL);
    if (status != 0)
    {
        perror(“pthread_create”);
        exit(1);
    }
    sleep(sec);
    pthread_cancel(thread_id);
    printf("counter = %d\n", ncount);
    return 0;
}
void *do_loop(void *loop)
{
    while (1)
        ncount++;
    return NULL;
}