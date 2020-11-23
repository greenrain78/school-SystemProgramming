#include <stdio.h>
#include <unistd.h>

void fork2();

int main(){
	fork2();
}

void fork2(){
	printf("L0\n");
	fork();
	printf("L1\n");
	fork();
	printf("bye\n");
}
