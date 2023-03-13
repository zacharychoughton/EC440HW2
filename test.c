#include "zthreadlib.h"
#include <setjmp.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h> 
#include <pthread.h>

int printtest(void* ptr) { 
    printf("\nstatus: %d", (*(tid *)ptr).status);
    return 0; 
}

int main(){
    tid athread; // thread ID structure. 
    const void *attr; //attricbute about thread, always null for this. 
    void *(*start_routine) (void*); //function for created thread to execute. 
    void *arg;

    attr = NULL; 
    void *ptr = &athread;
    int (*printtest_ptr)(void*) = &printtest; 

    zthread_create(athread, attr, printtest_ptr, ptr); 

    return 0;
}
