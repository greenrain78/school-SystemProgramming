#include <stdio.h>
#include <unistd.h>

int main(){
	for(int i=0;i<2;i++){
		if(fork()==0){
			printf("\nchild%d: pid=%d pgrp=%d\n", i, getpid(), getpgrp());
			while(1);
		}
	}
}
