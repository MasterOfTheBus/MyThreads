#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mythread.hpp"

using namespace mythreads;

void testthread() {
  printQueue();
  printf("thread 1\n");
  for (int i = 0; i < 1000; i++) {
    printf("%d ", i);
  }
  mythread_exit();
}

void testthread2() {
  printQueue();
  printf("thread 2\n");
  for (int i = 0; i < 525; i++) {
    printf("a ");
  }
  printf("\n");
  mythread_exit();
}

int main() {
  mythread_init();
  set_quantum_size(100);
  int id = mythread_create("t1", testthread, 16384);
  if (id == -1) {
    exit(1);
  }

  id = mythread_create("t2", testthread2, 16384);
  if (id == -1) {
    exit(1);
  }
  mythread_state();

  printQueue();
  printf("\n\n");

  runthreads();
  printf("\n\n");

  mythread_state();
}
