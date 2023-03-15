#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "ec440threads.h"

void* thread_start(void* arg) {
  printf("Thread started\n");
  sleep(2);
  printf("Thread finished\n");
  return NULL;
}

int main() {
  pthread_t tid;
  int ret = pthread_create(&tid, NULL, thread_start, NULL);
  if (ret != 0) {
    fprintf(stderr, "Error creating thread\n");
    exit(EXIT_FAILURE);
  }

  pthread_exit(NULL);
  printf("This line should never be reached\n");

  return 0;
}