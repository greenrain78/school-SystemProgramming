volatile int ncount;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int main(int argc, char *argv[])
{
    void *thread_result;
    int status, loop;
    pthread_t thread_id[2];
    ncount = 0;
    loop = atoi(argv[1]);
    status = pthread_create(&thread_id[0], NULL, do_loop1, &loop);
    status = pthread_create(&thread_id[1], NULL, do_loop2, &loop);
    /* FIXME: no sanity check */
    pthread_join(thread_id[0], &thread_result);
    pthread_join(thread_id[1], &thread_result);
    printf("counter = %d", ncount);
    pthread_mutex_destroy(&mutex);
    return 0;
}
void *do_loop1(void *data)
{
    int i, *loop = (int *)data;
    for (i = 0; i < *loop; i++)
    {
        pthread_mutex_lock(&mutex);
        ncount++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
void *do_loop2(void *data)
{
    int i, *loop = (int *)data;
    for (i = 0; i < *loop; i++)
    {
        pthread_mutex_lock(&mutex);
        ncount--;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}