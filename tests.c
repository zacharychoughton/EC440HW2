#include "ec440threads.h"
#include <setjmp.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h> 
#include <pthread.h>

void passfunc(void *args) { 
    printf("hi/n");
    return; 
}

int main(){
    pthread_t *athread;
    pthread_t thread1;  // thread ID structure. 
    athread = &thread1;
    const pthread_attr_t *attr; //attricbute about thread, always null for this. 
    void *args = 0; 
    attr = NULL; 
    void *(*passfunc_ptr)(void*) = *passfunc; 

    pthread_create(athread, attr,  passfunc_ptr, args); 

    return 0;
}
