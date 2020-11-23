#include <stdio.h>
#include <unistd.h>

void fork1();

int main(){
	fork1();
}

void fork1()
{
	int x = 1;
	pid_t pid = fork();
	if(pid == 0){
		printf("child has x = %d\n", ++x);
	}else{
		printf("parent has x = %d\n", --x);
	}
	printf("bye process %d with x = %d\n", getpid(), x);
}



