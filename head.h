#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

struct data_to_thread {
	int id;
	FILE* fp;
	int sec;
};
// writting in file from thread K N yyyy.mm.dd hh.mm.ss
void writting_to_file (struct data_to_thread* data);

//Function find and getting K from file 
int getting_K (FILE* fp);