int main()
{
    int fd1, fd2;
    char c;
    fd1 = open(“test.txt”, O_RDONLY);
    fd2 = open(“test.txt”, O_RDONLY);
    read(fd2, &c, 1);
    dup2(fd2, fd1);
    read(fd1, &c, 1);
    printf(“c = % c\n”, c);
    return 0;
}