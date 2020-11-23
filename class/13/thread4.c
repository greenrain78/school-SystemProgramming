volatile int ncount;

void *do_loop1(void *data);
void *do_loop2(void *data);

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
    return 0;
}
void *do_loop1(void *data)
{
    int i;
    int *loop = (int *)data;
    for (i = 0; i < *loop; i++)
        ncount++;
    return NULL;
}
void *do_loop2(void *data)
{
    int i;
    int *loop = (int *)data;
    for (i = 0; i < *loop; i++)
        ncount--;
    return NULL;
}