#include <stdio.h>
#include <mqueue.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NAMECP "/conv_message1"
#define NAMECC "/conv_message2"
#define NAMEPP "/pool_massage1"
#define NAMEPC "/pool_massage2"
#define CONV_SIZE 40
#define POOL_SIZE 20

void checkNum(int argc, char** argv, int* x, int* y);
void makeMatrix(int** matrix, int X, int Y);
int makeChild(int x, int y);

mqd_t openMessage(const char* name, size_t s);
void sendMessage(mqd_t mqdes, int* message, size_t s);
void receiveMessage(mqd_t mqdes, int* message, size_t s);

void ConvLayer(int** sample1, int** sample2, int x, int y);
void ConvCalcul(int* messagge);

void PoolLayer(int** sample2, int** sample3, int x, int y);
void Poolcalcul(int* message);

int main(int argc, char** argv) {
	int sizeX = 0;
	int sizeY = 0;
	int** sample1;
	int** sample2;
	int** sample3;

	//check Num
	checkNum(argc, argv, &sizeX, &sizeY);
	//make sample
	sample1 = (int**)malloc(sizeof(int*) * sizeX);
	for (size_t i = 0; i < sizeX; i++)
		sample1[i] = (int*)malloc(sizeof(int) * sizeY);

	sample2 = (int**)malloc(sizeof(int*) * (sizeX-2));
	for (size_t i = 0; i < (sizeX - 2); i++)
		sample2[i] = (int*)malloc(sizeof(int) * (sizeY - 2));

	sample3 = (int**)malloc(sizeof(int*) * ((sizeX - 2)/2));
	for (size_t i = 0; i < ((sizeX - 2) / 2); i++)
		sample3[i] = (int*)malloc(sizeof(int) * ((sizeY - 2) / 2));
	//make Matrix
	makeMatrix(sample1, sizeX, sizeY);

	printf("start Conv===========================\n");
	//Convolution Layer
	ConvLayer(sample1, sample2, sizeX, sizeX);

	printf(" start pooling=================================\n");
	//Max Polling Layer
	PoolLayer(sample2, sample3, sizeX-2, sizeY-2);

	printf("End Layer ==============\n");

	//check matrix
	printf("\n sample1\n");
	for (size_t i = 0; i < sizeX; i++) {
		for (size_t j = 0; j < sizeY; j++)
			printf("%d ", sample1[i][j]);
		printf("\n");
	}
	printf("\n sample2\n");
	for (size_t i = 0; i < sizeX - 2; i++) {
		for (size_t j = 0; j < sizeY - 2; j++)
			printf("%d ", sample2[i][j]);
		printf("\n");
	}
	printf("\n sample3\n");
	for (size_t i = 0; i < (sizeX - 2)/2; i++) {
		for (size_t j = 0; j < (sizeY - 2)/2; j++)
			printf("%d ", sample3[i][j]);
		printf("\n");
	}

	//free sample
	for (size_t i = 0; i < sizeX; i++)
		free(sample1[i]);
	free(sample1);

	for (size_t i = 0; i < sizeX - 2; i++)
		free(sample2[i]);
	free(sample2);

	for (size_t i = 0; i < (sizeX - 2)/2; i++)
		free(sample3[i]);
	free(sample3);
	printf("end program\n");
}

void checkNum(int argc, char** argv, int* x, int* y)
{
	if (argc == 2){
		int tmp = atoi(argv[1]);
		if (tmp % 2 == 0 && tmp >= 4) {
			(*x) = tmp;
			(*y) = tmp;
		}
	}
	else{
		printf("input wrong number\n");
		exit(0);
	}
	printf("X : %d, Y : %d\n", (*x), (*y));
}

int makeChild(int x, int y)
{
	for (size_t i = 0; i < x; i++)
	{
		for (size_t j = 0; j < y; j++)
		{
			printf("make child:  %ld %ld\n", (i + 1), (j + 1));
			if (fork() == 0) {
				return 0;
			}
		}
	}
	printf("end make child\n");
	return x*y;
}

mqd_t openMessage(const char* name, size_t s)
{
	struct mq_attr attr;
	mqd_t mqdes;

	//setting message queue
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = s;

	//open message queue
	mqdes = mq_open(name, O_CREAT | O_RDWR, 0600, &attr);
	if (mqdes < 0) {
		perror("mq_open()");
		exit(0);
	}

	return mqdes;
}

void sendMessage(mqd_t mqdes, int* message, size_t s)
{
	if (mq_send(mqdes, (char*)message, s, 0) == -1) {
		perror("mq_send()");
		exit(0);
	}
}

void receiveMessage(mqd_t mqdes, int* message, size_t s)
{	
	unsigned int prio;
	if (mq_receive(mqdes, (char*)message, s, &prio) == -1) {
		perror("mq_receive()");
		exit(0);
	}
}

void ConvLayer(int** sample1, int** sample2, int x, int y)
{
	mqd_t mqdesP;
	mqd_t mqdesC;
	int id = 0;
	int message[10];

	//open message queue
	mqdesP = openMessage(NAMECP, CONV_SIZE);
	mqdesC = openMessage(NAMECC, CONV_SIZE);
	//make child
	id = makeChild(x - 2, y - 2);
	
	//performance
	if (id == 0)
	{
		//receive
		receiveMessage(mqdesP, message, CONV_SIZE);
		printf("Conv child receive : %d\t", message[9]);
		for (size_t k = 0; k < 9; k++) {
			printf("%d ", message[k]);
		}
		printf("\n");
		//calcul
		ConvCalcul(message);
		//send
		printf("Conv child send : %d\n", message[0]);

		sendMessage(mqdesC, message, CONV_SIZE);
		printf("end child\n");
		exit(0);
	}
	else
	{
		int pos = 0;
		int child_status;
		//send message
		printf("parent send message");
		for (size_t i = 0; i < x - 2; i++) {
			for (size_t j = 0; j < y - 2; j++) {
				//cut Matrix
				int tmp = 0;
				for (size_t a = 0; a < 3; a++){
					for (size_t b = 0; b < 3; b++){
						message[tmp++] = sample1[i + a][j + b];
					}
				}
				message[9] = pos++;

				printf("parent send %ld , %ld : \t", i, j);
				for (size_t k = 0; k < 9; k++) {
					printf("%d ", message[k]);
				}
				printf("pos : %d\n", pos);
				//send
				sendMessage(mqdesP, message, CONV_SIZE);
			}
		}
		//wait & receive
		printf("receive from child\n");
		for (size_t i = 0; i < id; i++)
		{
			wait(&child_status);
			if (WIFEXITED(child_status))
			{   //correct
				receiveMessage(mqdesC, message, CONV_SIZE);
				//save
				printf("parent receive : ");
				for (size_t k = 0; k < 10; k++)
				{
					printf("%d ", message[k]);
				}
				printf("\n");
				sample2[message[9] / (x - 2)][message[9] % (y - 2)] = message[0];
			}
			else
			{   //error
				printf("child return error\n");
			}
		}
	}

	mq_close(mqdesP);
	mq_unlink(NAMECP);
	mq_close(mqdesC);
	mq_unlink(NAMECC);
}

void ConvCalcul(int* message)
{
	int sum = 0;
	int filter[9] = { -1, -1, -1, -1, 8, -1, -1, -1, -1 };
	for (size_t i = 0; i < 9; i++){
		sum += message[i] * filter[i];
	}
	message[0] = sum;
}

void PoolLayer(int** sample2, int** sample3, int x, int y)
{
	mqd_t mqdesP;
	mqd_t mqdesC;
	int id = 0;
	int message[5];

	//open message queue
	mqdesP = openMessage(NAMEPP, POOL_SIZE);
	mqdesC = openMessage(NAMEPC, POOL_SIZE);

	//make child
	id = makeChild(x / 2, y / 2);
	
	//performance
	if (id == 0)
	{
		//receive
		receiveMessage(mqdesP, message, POOL_SIZE);
		printf("receive : %d : \t ", message[5]);
		for (size_t k = 0; k < 5; k++) {
			printf("%d ", message[k]);
		}
		//calcul
		Poolcalcul(message);
		printf("Pool child send : %d\n", message[0]);
		//send
		sendMessage(mqdesC, message, POOL_SIZE);
		printf("end child\n");
		exit(0);
	}
	else
	{
		int pos = 0;
		int child_status;
		//send message
		for (size_t i = 0; i < x; i +=2) {
			for (size_t j = 0; j < y; j +=2) {
				//cut Matrix
				int tmp = 0;
				for (size_t a = 0; a < 2; a++) {
					for (size_t b = 0; b < 2; b++) {
						message[tmp++] = sample2[i + a][j + b];
					}
				}
				message[4] = pos++;

				printf("Pool parent send %ld , %ld : \t", i, j);
				for (size_t k = 0; k < 5; k++) {
					printf("%d ", message[k]);
				}
				printf("pos : %d\n", pos);
				//send
				sendMessage(mqdesP, message, POOL_SIZE);
			}
		}
		//wait & receive
		for (size_t i = 0; i < id; i++)
		{
			wait(&child_status);
			if (WIFEXITED(child_status))
			{   //correct
				receiveMessage(mqdesC, message, POOL_SIZE);
				//save
				printf(" pool parent receive : ");
				for (size_t k = 0; k < 5; k++)
				{
					printf("%d ", message[k]);
				}
				printf("\n");
				sample3[message[4] / (x/2)][message[4] % (y/2)] = message[0];
			}
			else
			{   //error
				printf("child return error\n");
			}
		}
	}
	mq_close(mqdesP);
	mq_unlink(NAMEPP);
	mq_close(mqdesC);
	mq_unlink(NAMEPC);
}

void Poolcalcul(int* message)
{
	int max = message[0];
	for (size_t i = 1; i < 4; i++){
		if (max < message[i]){
			max = message[i];
		}
	}
	message[0] = max;
}
