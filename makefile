override CFLAGS :=  -Werror -Wall -std=gnu99 -O0 -g $(CFLAGS) -I.
CC = gcc

# prog = threads
# c_files = threads.c
# h_files = ec440threads.h 


# all: $(prog)

#jmp1 : jmp1.o ec440threads.h 
#test : test.o ec440threads.h
threads.o: threads.c ec440threads.h
#sched_simple : sched_simple.o

# .PHONY: clean 

test_c_files=$(shell find tests -type f -name '*.c')
test_o_files=$(test_c_files:.c=.o)
test_files=$(test_c_files:.c=)

tests/%: tests/%.o threads.o
	$(CC) $(LDFLAGS) $+ $(LOADLIBES) $(LDLIBS) -o $@

checkprogs: $(test_files)

check: checkprogs
	tests/run_tests.sh $(test_files)

clean:
	rm -f *.o $(test_files) $(test_o_files)

# clean:
# 	rm -f *.o *~ $(prog)