override CFLAGS :=  -Werror -Wall -std=gnu99 -O0 -g $(CFLAGS) -I.
CC = gcc

prog = test threads
c_files = test.c threads.c
h_files = ec440threads.h 


all: $(prog)

#jmp1 : jmp1.o ec440threads.h 
#test : test.o ec440threads.h
threads: threads.o ec440threads.h
#sched_simple : sched_simple.o

.PHONY: clean 

clean:
	rm -f *.o *~ $(prog)