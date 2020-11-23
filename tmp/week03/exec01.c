#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
	if(fork() == 0){
		execl("/usr/bin/cp", "cp", "foo", "bar", 0);
	}
	wait(NULL);
	printf("copy completed\n");
	exit(0);
}
