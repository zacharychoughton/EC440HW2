#include "ec440threads.h"
#include <setjmp.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h> 
#include <pthread.h>

int passfunc(void *args) { 
    return 0; 
}

int main(){
    pthread_t *athread; // thread ID structure. 
    const pthread_attr_t *attr; //attricbute about thread, always null for this. 
    void *args = 0; 
    attr = NULL; 
    int (*passfunc_ptr)(void*) = &passfunc; 

    pthread_create(&athread, attr, passfunc_ptr, args); 

    return 0;
}
