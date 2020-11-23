#include <stdio.h>
#include <unistd.h>

void fork3();


int main(){
	fork3();
}

void fork3(){

	printf("L0\n");
	fork();
	printf("L1\n");
	fork();
	printf("L2\n");
	fork();
	printf("bye\n");

}
