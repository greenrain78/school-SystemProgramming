#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void cleanup(void);
void fork6();

int main(){
	fork6();
}

void fork6(){
	atexit(cleanup);
	fork();
	exit(0);
}

void cleanup(void){
	printf("cleaning up\n");
}
