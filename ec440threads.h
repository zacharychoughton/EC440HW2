
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

void pthread_create_helper(); 
//Initilizes threads, sets up scheduler, and TCB.

void schedule(void ); 
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

#endif