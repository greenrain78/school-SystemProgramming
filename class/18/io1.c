int main(void)
{
    fd_set readfds;
    int ret;
    /* Wait on stdin for input. */
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    /* All right, now block! */
    ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, NULL);
    if (ret == -1)
    {
        perror("select");
        return 1;
    }
    if (FD_ISSET(STDIN_FILENO, &readfds))
    {
        char buf[BUF_LEN + 1];
        int len;
        /* guaranteed to not block */
        len = read(STDIN_FILENO, buf, BUF_LEN);
        if (len == -1)
        {
            perror("read");
            return 1;
        }
        if (len)
        {
            buf[len] = '\0';
            printf("read: %s\n", buf);
        }
        return 0;
    }
    printf("This should not happen!\n");
    return 1;
}