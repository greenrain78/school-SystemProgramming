#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>


int main(int argc, char *argv[])
{
    struct stat sb;
    off_t len;
    char *p;
    int fd;

    //인자 없을 시 실행 파일명 출력
    if (argc < 2)
    {
        printf("usage: %s <file>\n", argv[0]);
        return 1;
    }
    //open
    fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        return 1;
    }
    //파일 정보를 가져오기
    if (fstat(fd, &sb) == -1)
    {
        perror("fstat");
        return 1;
    }
    //레귤러 파일인지 확인
    if (!S_ISREG(sb.st_mode))
    {
        printf("%s is not a file\n", argv[1]);
        return 1;
    }

    //mmap
    p = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED)
    {
        perror("mmap");
        return 1;
    }

    //close
    if (close(fd) == -1)
    {
        perror("close");
        return 1;
    }

    //print
    for (len = 0; len < sb.st_size; len++)
        putchar(p[len]);
        
    if (munmap(p, sb.st_size) == -1)
    {
        perror("munmap");
        return 1;
    }
    return 0;
}