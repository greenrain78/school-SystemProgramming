int main(void)
{
	char str[BUF_SIZE], rbuf[BUF_SIZE];
	size_t len;
	ssize_t ret;
	int fd;
	char *bufp = rbuf;
	printf("Input: ");

	if ( fgets( str, BUF_SIZE, stdin ) == NULL ){
		perror("fgets");
		exit(1);
	}
	len = strlen(str);
	if ( fork() == 0 ){
		fd = open("file1.txt", O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
		if ( fd < 0 ){
			perror("open");
			exit(1);
		}
		if ( write(fd, str, len) < 0 ){
			perror("write");
			close(fd);
			exit(1);
		}
	}
	else{
	wait(NULL);
	fd = open("file1.txt", O_RDONLY);
		if ( fd < 0 ){
			perror("open");
			exit(1);
		}
		while(len){
			ret = read(fd, bufp, len);
			if ( ret < 0 ){
				if ( errno == EINTR )
					continue;
				perror("read");
				close(fd);
				exit(1);
			}
			len -= ret;
			bufp += ret;
		}
		*bufp = '\0';
		printf("Output: %s\n", rbuf);
	}
	close(fd);
	return 0;
}