// Creates a new thread to run process start_routine. Initializes threas subsystem uppon first call. 
#include "ec440threads.h"
#include <setjmp.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h> 
#include <pthread.h>
#include <unistd.h>
#include <strings.h>
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

#define max_threads 128 //max number of threads

#define quanta (50*1000) // quanta: 50 ms for each thread executuion time 

static int first_time = 1; // determines if helper is called 

int currentthread = 0; //current thread index

static int mainthread = 0; // to see if we are in main process or thread

static int numthreads = 0; //number of threads running 

struct sigaction sighandler, oldsighandler; 
stack_t stack, oldstack; 

jmp_buf alphathread; 

struct TCB{ 
    int status; //0 = exited, 1 = running, 2 = ready, 3 = blocked, 4 = unused
    void* sp; //stack pointer 
    jmp_buf regs; // execution context. 
    pthread_t threadid; //thread ID 
}; 

struct TCB TCBlist[ max_threads ]; //List of TCBs. 

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
        TCBlist[0].status = 2; //ready
        mainthread = setjmp(TCBlist[0].regs); //return 0 on set, returns nonzero integer on return from longjmp. 
    } //initilizes thread subsystem. 

    if (numthreads == max_threads){
        printf("Did not thread. Max number of threads reached.\n");
        return -1;
    } //if max number of threads reach, returns -1 and does not make a new thread.

    if(!mainthread){ 
    
    currentthread = 1; 
    while(TCBlist[currentthread].status != 0){
        currentthread++; 
    }

    *thread = TCBlist[currentthread].threadid;

    setjmp(TCBlist[currentthread].regs);

    TCBlist[currentthread].regs[JB_PC] = ptr_mangle((unsigned long int)start_thunk);
    //.__jmpbuf[JB_PC] = ptr_mangle((unsigned long int)start_thunk);

    TCBlist[currentthread].regs[JB_R13] = (long) arg; 

    TCBlist[currentthread].regs[JB_R12] = (unsigned long int) start_routine; 

    }
    else{
        mainthread =0; 
    }
    
    
    //TCBlist[0].threadid;
    
    
    //Creates a stack 
    stack.ss_flags = 0; 
    stack.ss_size = STACK_SIZE; 
    stack.ss_sp = malloc(STACK_SIZE); 
    if (stack.ss_sp == 0) {printf("Error allocating stack"); return -2;}

    sigaltstack(&stack, &oldstack); 

    // sighandler.sa_handler = &SIGUSR1Handler;  
    // sighandler.sa_flags = SA_ONSTACK; 
    // sigemptyset(&sighandler.sa_mask); 
    // sigaction(SIGUSR1, &sighandler, &oldsighandler);
    // raise(SIGUSR1); 

    // sigaltstack(&oldstack,0); 
    // sigaction(SIGUSR1, &oldsighandler, 0);

    TCBlist[numthreads].status = 1;
    TCBlist[numthreads].sp = stack.ss_sp; 

    numthreads ++; 

    schedule(SIGALRM); 

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
    }
    
    struct timeval timeint, timeval;
    timeint.tv_usec = quanta; 
    timeval.tv_usec = quanta; 
    struct itimerval timing;
    timing.it_interval = timeint;
    timing.it_value = timeval;
    const struct itimerval* timeptr = &timing; 
    

    s = setitimer(1,timeptr,NULL); //sets up regular SIGALRM intervals. 
    if (s == -1){ 
        printf("Error creating timer\n");
        exit(1); 
    }

    //respond to SIGALRM. -> SIGALRM calls schedule. 
    sigemptyset(&sighandler.sa_mask);
    sighandler.sa_handler = &schedule; 
    sighandler.sa_flags = SA_NODEFER; 
    sigaction(SIGALRM, &sighandler, NULL); 
} 

/**********************schedule************************/
void schedule(int sig){ 
    if(TCBlist[currentthread].status ==1) /*running)*/{
    TCBlist[currentthread].status = 2; /*ready*/
    }

    // Find Next Ready Thread 
    int FindID = currentthread; 
    FindID++; 
    while(1){
        if(FindID == max_threads-1){
            FindID = 0; 
        }
        else{
            FindID++; 
        }

        if(TCBlist[currentthread].status == 2){
            break; 
        }
    }

    // Saves current thread context 
    int setup = 0; 
    if( TCBlist[FindID].status != 0){
        setup = setjmp(TCBlist[FindID].regs);
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
    pthread_t pid = 1; 
    return pid;
}

/******************pthread_exit****************************/
void pthread_exit(void *value_ptr){ 
    //free stacks. 
    /*Install signal handler for when start_routine returns
    and pass return value of routine as exit status. 
    and when mainthread returns, call normal exit.
    Remember to decrement numthreads*/ 
    exit(0); 
}