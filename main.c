/*
* @file main.c
* @author Renat Kagal <kagal@itspartner.net>
*
* Assembling : gcc -Wall main.c -o main
*
* Description : program find words with yours characters, reverse text and counting sentensces and words in each sentence
*
* Copyright (c) 2021, ITS Partner LLC.
* All rights reserved.
*
* This software is the confidential and proprietary information of
* ITS Partner LLC. ("Confidential Information"). You shall not
* disclose such Confidential Information and shall use it only in
* accordance with the terms of the license agreement you entered into
* with ITS Partner.
*/

#include "head.h"

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

#define MAX_STRING_SIZE_FOR_DATA 20

time_t begin_time;

int main (int argc, char** argv) {
	int sec = 10;
	char* file_name = "text.txt";
	int K = 5;

	if (argc < 2) ;
	else if (argc < 3){
		sec = atoi (argv[1]);
	}
	else if (argc < 4) {
		sec = atoi (argv[1]);
		file_name = argv [2];
	}
	else {
		sec = atoi (argv[1]);
		file_name = argv[2];
		K = atoi (argv[3]);
	}

	size_t N = 0;
	scanf ("%ld", &N);

	pthread_t array_threads [N];
    FILE* fp;

    if ((fp = fopen (file_name, "w+")) == NULL) {
        puts ("Failed opne file");
        exit (EXIT_FAILURE);
    }
    fprintf (fp, "%d", K);

    struct data_to_thread* data;
    begin_time = time (NULL);

    for (int i = 0; i < N; i++) {
    	data = (struct data_to_thread*) malloc (sizeof(struct data_to_thread));
    	data->fp = fp;
    	data->id = i + 1;
    	data->sec = sec;

    	if (pthread_create (&array_threads[i], NULL, writting_to_file, data)) {
    		printf ("Failed creating thread : %d\n", data->id);
    		exit (EXIT_FAILURE);
    	}
    }
    for (int i = 0; i < N; i++) {
    	if (pthread_join (array_threads[i], NULL)) {
    		printf("Failed join thread id : %d\n", (i + 1));
    		exit (EXIT_FAILURE);
    	}
    }
    if (fclose (fp)) {
    	puts ("Failed closing file");
    	exit (EXIT_FAILURE);
    }
}

// writting in file from thread K N yyyy.mm.dd hh.mm.ss
void writting_to_file (struct data_to_thread* data) {

	time_t time_thread = time (NULL);
	struct tm* tm_ptr = localtime (&time_thread);

	char time_string [MAX_STRING_SIZE_FOR_DATA];
	int K;

	while (difftime (time_thread, begin_time) < data->sec) {
		if (pthread_mutex_lock (&mtx)) {
			puts ("Failed lock mutex");
			exit (EXIT_FAILURE);
		}
		fseek (data->fp, 24, 2);
		fscanf (data->fp, "%d", &K);

		if (strftime (time_string, MAX_STRING_SIZE_FOR_DATA, "%Y.%m.%d %H.%M.%S", tm_ptr) == 0) {
		puts ("Failed writting data");
		exit (EXIT_FAILURE);
		}
		fseek (data->fp, 0, 2);
		fprintf(data->fp, "%d %d %s\n", K, data->id, time_string);

		if (pthread_mutex_unlock(&mtx)) {
			puts ("Failed unlock mutex");
			exit (EXIT_FAILURE);
		}
		time_thread = time (NULL);
		tm_ptr = localtime (&time_thread);
	}

	free (data);
}

/*
Есть файл, в котором записано некоторое начальное число K >=0. Напистаь программу, которая создает N потоков (не более 9). Один из потоков считывает это число, 
увеличивает его на еденицу и записывает
в файл строку вида "K N yyyy.mm.dd hh.mm.ss" где N - id потока. Следуйщий поток опять считывает из предыдущий записи K и делает тоже самое и т.д.
 Время работы программы(сек), имя файла и начальное значение
K задать через аргументы запуска.
*/