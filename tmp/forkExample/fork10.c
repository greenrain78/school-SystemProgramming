#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
void fork10();

int main(int argc, char** argv){
	printf("\nfork : %s\n", argv[1]);
	printf("%d\n", (int)(*argv[1]-48));
	switch((int)(*argv[1])-48){
	case 1:
		fork10();
		break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
		printf("dont write here");
		break;
	}
}

void fork10(){
	int N = 100;
	pid_t pid[N];
	int i, child_status;
	for(i=0; i<N;i++)
		if((pid[i] = fork())==0)
			exit(100+i);
	for(i=0; i<N;i++){
		pid_t wpid = wait(&child_status);
		if(WIFEXITED(child_status))
			printf("child %d terminated with exit status %d\n", wpid, WEXITSTATUS(child_status));
		else
			printf("Child %d terminate abnomally\n", wpid);
	}
}




