#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void fork7();

int main(){
	fork7();
}
void fork7(){
	if(fork()==0){
		printf("terminating child, pid = %d\n", getpid());
		exit(0);
	}else{
		printf("Running Parent, pid = %d\n", getpid());
		while(1)
			;
	}
}
