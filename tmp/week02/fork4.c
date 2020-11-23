#include <stdio.h>
#include <unistd.h>

void fork4();

int main(){
	fork4();
}

void fork4(){
	printf("L0\n");
	if(fork()!=0){
		printf("L1\n");
		if(fork()!=0){
			printf("L2\n");
			fork();
		}
	}
	printf("bye\n");
}

