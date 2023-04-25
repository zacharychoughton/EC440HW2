#include "ec440threads.h"
#include <setjmp.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h> 
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 3

pthread_barrier_t barrier;

void* thread_func(void* arg) {
    int tid = *(int*)arg;
    printf("Thread %d is starting...\n", tid);
    // Wait for all threads to start before proceeding
    pthread_barrier_wait(&barrier);
    printf("Thread %d is now running.\n", tid);
    // Wait for all threads to finish before proceeding
    pthread_barrier_wait(&barrier);
    printf("Thread %d is now exiting.\n", tid);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int tids[NUM_THREADS];
    int i, rc;
    
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);
    
    for (i = 0; i < NUM_THREADS; i++) {
        tids[i] = i;
        rc = pthread_create(&threads[i], NULL, thread_func, &tids[i]);
        if (rc) {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
    
    // Wait for all threads to finish before exiting main
    pthread_barrier_wait(&barrier);
    
    printf("All threads are now running.\n");
    
    // Wait for all threads to finish before exiting program
    pthread_barrier_wait(&barrier);
    
    printf("All threads have now exited.\n");
    
    pthread_barrier_destroy(&barrier);
    
    return 0;
}