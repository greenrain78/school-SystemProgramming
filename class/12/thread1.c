#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void *thread_routine(void *arg)
{
    pid_t my_pid;
    pthread_t my_tid;
    my_pid = getpid();
    my_tid = pthread_self();
    printf("New thread: pid % d, tid % lu\n", my_pid, my_tid);
    return arg;
}

int main()
{
    pthread_t thread_id, my_tid;
    pid_t my_pid;
    void *thread_result;
    int status;
    my_pid = getpid();
    my_tid = pthread_self();
    printf("Main thread: pid % d, tid % lu\n", my_pid, my_tid);
    status = pthread_create(&thread_id, NULL, thread_routine, NULL);
    if (status != 0)
    {
        perror("pthread_create");
        exit(1);
    }
    status = pthread_join(thread_id, &thread_result);
    if (status != 0)
    {
        perror("pthread_join");
        exit(1);
    }
    return 0;
}