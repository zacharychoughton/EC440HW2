// Creates a new thread to run process start_routine. Initializes threas subsystem uppon first call. 
#include "ec440threads.h"
#include <setjmp.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h> 
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

/**********************************************/

#define JB_RBX 0
#define JB_RBP 1
#define JB_R12 2
#define JB_R13 3
#define JB_R14 4
#define JB_R15 5
#define JB_RSP 6
#define JB_PC 7

#define _XOPEN_SOURCE 500

#define STACK_SIZE 32767

#define max_threads 5 //max number of threads

#define quanta 50000 // quanta: 50 ms (50,000 micro s) for each thread executuion time 

static int first_time = 1; // determines if helper is called 

pthread_t currentthread = 0; //current thread index

static int mainthread = 0; // to see if we are in main process or thread (1 if yes, 0 if no)

static int numthreads = 1; //number of threads running 

struct sigaction sighandler, oldsighandler; 

//jmp_buf alpha; //main context 

struct TCB{ 
    int status; //0 = exited, 1 = running, 2 = ready, 3 = blocked, 4 = unused
    void* sp; //stack pointer 
    jmp_buf regs; // execution context. 
    pthread_t threadid; //thread ID 
}; 

static struct TCB TCBlist[ max_threads ]; //List of TCBs. 

/*******************pthread_create***************************/
int pthread_create(
    pthread_t *thread,
    const pthread_attr_t *attr, 
    void *(*start_routine)(void*), 
    void *arg)
{ 
    if (first_time){
        pthread_create_helper(); 
        first_time = 0;
        TCBlist[currentthread].status = 1; //running
        mainthread = setjmp(TCBlist[currentthread].regs); //return 0 on set, returns nonzero integer on return from longjmp. 
        //TCBlist[0].regs has main context. 
    } //initilizes thread subsystem. 

    if (numthreads == max_threads){
        printf("Did not thread. Max number of threads reached.\n");
        return -1;
    } //if max number of threads reach, returns -1 and does not make a new thread.

    pthread_t newthread = currentthread; 

    if(!mainthread){ 

    while(TCBlist[newthread].status != 4){
        newthread++; 
    }

    thread = &TCBlist[newthread].threadid;

    setjmp(TCBlist[newthread].regs);

    TCBlist[newthread].regs->__jmpbuf[JB_PC] = ptr_mangle((unsigned long int)start_thunk); //sets program counter. 
    //starthunk moves value of R13 to RDI. Then jumps to address of R12. so R13 should be arg, and R12 should be function. 
    TCBlist[newthread].regs->__jmpbuf[JB_R13] = (long) arg; 

    TCBlist[newthread].regs->__jmpbuf[JB_R12]  = (unsigned long int) start_routine; 

    TCBlist[newthread].sp = malloc(STACK_SIZE); //malloc points to bottom of stack. 

    //Want sp to point to top of new stack. (stacks grow down for us).
    void* bottom = TCBlist[newthread].sp + STACK_SIZE; 
    void* stack = bottom - sizeof(&pthread_exit); 
    void (*temp)(void*) = (void*) &pthread_exit; //pthread exit will be called when start_routine returns. 
    stack = memcpy(stack,&temp, sizeof(temp)); 

    TCBlist[newthread].regs[0].__jmpbuf[JB_RSP] = ptr_mangle((unsigned long int)stack);

    TCBlist[newthread].status = 2; 
    TCBlist[newthread].threadid = newthread; 

    numthreads++; 

    schedule(SIGALRM); 

    }
    else{
        mainthread =0; 
    }
    
    
    //TCBlist[0].threadid;
    


    // sighandler.sa_handler = &SIGUSR1Handler;  
    // sighandler.sa_flags = SA_ONSTACK; 
    // sigemptyset(&sighandler.sa_mask); 
    // sigaction(SIGUSR1, &sighandler, &oldsighandler);
    // raise(SIGUSR1); 

    // sigaltstack(&oldstack,0); 
    // sigaction(SIGUSR1, &oldsighandler, 0);

    TCBlist[numthreads].status = 2; //ready 

    numthreads ++; 

 return 0; 
} 

/**********************pthread_create_helper************************/
void pthread_create_helper(){
   // This function is called only when pthread_create is called for the first time.
   // First, it initializes the thread array, all relevant data structures, and a stack space.  
   // It sets up a stack space using malloc for the threads

    int i,s; 

    for (i=0 ; i < max_threads ; ++i) {
        TCBlist[ i ].status = 4; //0 = exited, 1 = running, 2 = ready, 3 = blocked, 4 = unused
        TCBlist[ i ].sp = 0;
        TCBlist[i].threadid = 0; 
    }
    
    //setting up timer. 
    struct timeval timeint, timeval;
    timeint.tv_usec = quanta; 
    timeint.tv_sec = quanta/1000000; 
    timeval.tv_usec = quanta; 
    timeval.tv_sec = quanta/1000000; 
    struct itimerval timing;
    timing.it_interval = timeint;
    timing.it_value = timeval;
    const struct itimerval* timeptr = &timing; 

    s = setitimer(ITIMER_VIRTUAL,timeptr,NULL); //sets up regular SIGALRM intervals. 
    if (s == -1){ 
        printf("Error creating timer\n");
        exit(1); 
    }

    //respond to SIGALRM. -> SIGALRM calls schedule. 
    sigemptyset(&sighandler.sa_mask);
    sighandler.sa_handler = &schedule; 
    sighandler.sa_flags = SA_NODEFER; 
    sigaction(SIGVTALRM, &sighandler, NULL); 
} 

/**********************schedule************************/
void schedule(int sig){ 
    if(TCBlist[currentthread].status ==1) /*running)*/{
    TCBlist[currentthread].status = 2; /*ready*/
    }


    // Find Next Ready Thread 
    pthread_t FindID = currentthread; 
    FindID++; 

    while(1){
        if(FindID == max_threads+1){
            FindID = 0; 
        }
        else if (numthreads!= 0){

            if(TCBlist[FindID].status == 2){
            break; 
            }

            FindID++; 
        }

    }

    // Saves current thread context 
    int setup = 0; 
    if( TCBlist[currentthread].status != 0){
        setup = setjmp(TCBlist[currentthread].regs);
    }

    //Runs Thread
    if(!setup){
        currentthread = FindID; 
        TCBlist[currentthread].status = 1; 
        longjmp(TCBlist[currentthread].regs,1);
    } 
}

/*******************zthread_self***************************/
pthread_t pthread_self(void){
    return currentthread;
}

/******************pthread_exit****************************/
void pthread_exit(void *value_ptr){ 

    TCBlist[currentthread].status = 0; //exited status. 

    numthreads--; 
    //int i,j=0;

    free(TCBlist[currentthread].sp);
    TCBlist[currentthread].status = 4; 
    TCBlist[currentthread].threadid = 0;
    schedule(SIGALRM);

    // for (i = 0; i<max_threads;i++){ 
    //     if(TCBlist[i].status == 3 /*blocked*/){
    //         j = 1; 
    //         break; 
    //         }
    //     else if (TCBlist[i].status == 4 ){
    //             break; 
    //         }
    // }

    // if(j){ 
    //     schedule(SIGALRM); 
    // }

    // for( i = 0; i< max_threads; i++){
    //     if(TCBlist[i].status == 0){
    //         free(TCBlist[i].sp);
    //         TCBlist[i].status = 4; //reset to unused.
    //         TCBlist[i].threadid = 0; 
    //     }
    // }
    exit(0);
}
