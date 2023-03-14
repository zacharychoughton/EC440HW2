#include "ec440threads.h"
#include <setjmp.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h> 
#include <pthread.h>
#include <unistd.h>

void passfunc(void *args) { 
    printf("hi 1\n");
    sleep(3);
    printf("hi 2\n");
    return; 
}

int main(){
    pthread_t *athread[100];
    //pthread_t thread1[100];  // thread ID structure. 
    //athread = &thread1;
    const pthread_attr_t *attr; //attricbute about thread, always null for this. 
    void *args = 0; 
    attr = NULL; 
    void *(*passfunc_ptr)(void*) = &passfunc; 
    int i; 

    for( int i = 0; i<100; i++);{
    pthread_create(athread[i], attr,  passfunc_ptr, args); 
    }

    return 0;
}
