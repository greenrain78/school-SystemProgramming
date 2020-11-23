int main(void)
{
    fd_set readfds, writefds;
    int ret, fd;
    /* Wait on stdin for input. */
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(STDIN_FILENO, &readfds);
    FD_SET(STDOUT_FILENO, &writefds);
    /* All right, now block! */
    ret = select(STDOUT_FILENO + 1, &readfds, &writefds, NULL, NULL);
    if (ret == -1)
    {
        perror("select");
        return 1;
    }
    for (fd = 0; fd < STDOUT_FILENO + 1; fd++)
printf(“fd %d: %s%s\n", fd
FD_ISSET(fd, &readfds) ? “R" : "",
FD_ISSET(fd, &writefds) ? “W" : "");
return 1;
}