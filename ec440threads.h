
#ifndef __EC440THREADS__
#define __EC440THREADS__

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

// ***********Function Headers ***********
int pthread_create(
    pthread_t *athread, 
    const pthread_attr_t *attr, 
    void *(*start_routine)(void*), 
    void *arg);
//Creates a new thread stored in address athread. 

void pthread_create_helper(void); 
//Initilizes threads, sets up scheduler, and TCB.

void schedule(int signal); 
//Schedules the threads in round robid fashion.  

pthread_t pthread_self(void); 
//Returns id of current thread. 

void pthread_exit(void* value_ptr); 
//Exits thread and frees stack/related memory. 

static unsigned long int ptr_demangle(unsigned long int p) __attribute__((unused));
// Unmangles Pointer to 

//*********** Helper Functions *************
static void *start_thunk() __attribute__((unused));

static unsigned long int ptr_mangle(unsigned long int p) __attribute__((unused)); 

static unsigned long int ptr_demangle(unsigned long int p) 
{
    unsigned long int ret;

    asm("movq %1, %%rax;\n"
        "rorq $0x11, %%rax;"
        "xorq %%fs:0x30, %%rax;"
        "movq %%rax, %0;"
    : "=r"(ret)
    : "r"(p)
    : "%rax"
    );
    return ret;
}

static unsigned long int ptr_mangle(unsigned long int p)
{
    unsigned long int ret;

    asm("movq %1, %%rax;\n"
        "xorq %%fs:0x30, %%rax;"
        "rolq $0x11, %%rax;"
        "movq %%rax, %0;"
    : "=r"(ret)
    : "r"(p)
    : "%rax"
    );
    return ret;
}



static void *start_thunk()
{
  asm("popq %%rbp;\n"           //clean up the function prologue
      "movq %%r13, %%rdi;\n"    //put arg in $rdi
      "pushq %%r12;\n"          //push &start_routine
      "retq;\n"                 //return to &start_routine
      :
      :
      : "%rdi"
  );
  __builtin_unreachable();
}

///////////////// Synchornization //////////////
    /// Structures 

typedef struct waiting_list{
    pthread_t tid;
    struct waiting_list* next;
    struct waiting_list* prev;
}waiting_list; 

typedef enum{
    locked,
    unlocked
}mutex_position; 

typedef struct{ 
    mutex_position position;
    waiting_list *line;
}mutexstruct;

typedef struct{
        char init; 
        char flag;
        pthread_t calling_thread;
        unsigned count;
        unsigned left; 
}barrier;

///Mutex Functions

int pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attr);

int pthread_mutex_destroy(pthread_mutex_t *mutex);

int pthread_mutex_lock(pthread_mutex_t *mutex);

int pthread_mutex_unlock(pthread_mutex_t *mutex); 

///Barrier Functions 

int pthread_barrier_init(pthread_barrier_t *restrict barrier, const pthread_barrierattr_t *restrict attr, unsigned count); 

int pthread_barrier_destroy(pthread_barrier_t *barrier); 

int pthread_barrier_wait(pthread_barrier_t *barrier); 

    ///Recomended Internal Functions

static void lock(); 
// Disable timer that calls schedule routine.

static void unlock(); 
// Re-enable timer. 

void remove_from_waiting_list(waiting_list **head, pthread_t *tid);

void add_to_waiting_list(waiting_list** head, pthread_t tid);

#endif