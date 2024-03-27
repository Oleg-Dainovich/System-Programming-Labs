#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_THREADS 2
#define FILENAME "output.txt"
#define BILLION 1000000000L
#define SYMBOL_NUM 20

void *thread_function(void *arg) {
    FILE *file = fopen(FILENAME, "a");
    if (file == NULL) {
        printf("Error in file opening: %s\n", FILENAME);
        pthread_exit(NULL);
    }

    for(int i = 0; i < SYMBOL_NUM / NUM_THREADS; i++){
    	fprintf(file, "%d", i);
    	sleep(1);
    }
    
    fprintf(file, "\n");
    
    fclose(file);
    pthread_exit(NULL);
}

int main() {
    FILE* file = fopen(FILENAME, "w");
    if (file == NULL) {
        printf("Error in file opening: %s\n", FILENAME);
        exit(-1);
    }
    fclose(file);
    
    if (NUM_THREADS > SYMBOL_NUM || SYMBOL_NUM % NUM_THREADS != 0) {
    	printf("Error. Number of threads is invalid.\n");
    	exit(-1);
    }

    pthread_t threads[NUM_THREADS];

    int result;
    
    unsigned long long diff;
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC_RAW , &start);

    for (int i = 0; i < NUM_THREADS; i++) {
        result = pthread_create(&threads[i], NULL, thread_function, NULL);
        if (result) {
            printf("Error in thread opening: %d\n", result);
            exit(-1);
        }
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        result = pthread_join(threads[i], NULL);
        if (result) {
            printf("Error in thread closing: %d\n", result);
            exit(-1);
        }
    }

    clock_gettime(CLOCK_MONOTONIC_RAW , &end);
    diff = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
    printf("Number of threads: %d \n", NUM_THREADS);
    printf("Parallel writing in file time = %lld ns\n", diff);
    printf("Writing in file is finished.\n");

    return 0;
}

