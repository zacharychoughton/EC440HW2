override CFLAGS :=  -std=gnu99 -O0 -g $(CFLAGS) -I.
CC = gcc

threads.o: threads.c ec440threads.h
