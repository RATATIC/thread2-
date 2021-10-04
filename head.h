#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

struct data_to_thread {
	int id;
	FILE* fp;
	int sec;
};

void writting_to_file (struct data_to_thread* data);