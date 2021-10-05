/*
* @file main.c
* @author Renat Kagal <kagal@itspartner.net>
*
* Assembling : gcc -Wall main.c -o main
*
* Description : 
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

#define READING_SIZE 2
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

    while (scanf ("%ld", &N)) {
        if (N <= 9 && N > 0) {
            break;
        } 
        puts ("Bad N entet again > 0 and <= 9");
    }
    pthread_t array_threads [N];
    FILE* fp;

    if ((fp = fopen (file_name, "w+")) == NULL) {
        puts ("Failed opne file");
        exit (EXIT_FAILURE);
    }
    fprintf (fp, "\n%d\n", K);

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

    while ( (clock () / CLOCKS_PER_SEC ) < data->sec) {
        if (pthread_mutex_lock (&mtx)) {
            puts ("Failed lock mutex");
            exit (EXIT_FAILURE);
        }
        K = getting_K (data->fp) + 1;

        if (strftime (time_string, MAX_STRING_SIZE_FOR_DATA, "%Y.%m.%d %H.%M.%S", tm_ptr) == 0) {
        puts ("Failed writting data");
        exit (EXIT_FAILURE);
        }
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

//Function find and getting K from file 
int getting_K (FILE* fp) {
    char str[READING_SIZE];

    while (1) {
        if (fseek (fp, -READING_SIZE, SEEK_CUR)) {
            puts ("Failed fseek in while");
            exit (EXIT_FAILURE);
        }
        fgets (str, READING_SIZE, fp);

        if (!strcmp (str, "\n")) {
            break;
        }
    }
    int K = 0;
    fscanf (fp, "%d", &K);

    if (fseek (fp, 0, SEEK_END)) {
        puts ("Failed fseek");
        exit (EXIT_FAILURE);
    }
    return K;
}