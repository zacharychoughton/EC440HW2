override CFLAGS :=  -Werror -Wall -std=gnu99 -O0 -g $(CFLAGS) -I.
CC = gcc

# prog = threads
# c_files = threads.c
# h_files = ec440threads.h 


# all: $(prog)

#jmp1 : jmp1.o ec440threads.h 
#test : test.o ec440threads.h
threads.o: threads.o ec440threads.h
#sched_simple : sched_simple.o

# .PHONY: clean 

# clean:
# 	rm -f *.o *~ $(prog)