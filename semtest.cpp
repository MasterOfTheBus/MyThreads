#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mythread.hpp"

using namespace mythreads;

int value = 0;

void testthread() {
//  value++;
  //printf("value: %d\n", value);
  printQueue();
  printf("thread 1\n");
#if 1 
  for (int i = 0; i < 1000; i++) {
    printf("%d ", i);
  }
#endif
  mythread_exit();
}

void testthread2() {
  //printf("value: %d\n", value);
  printQueue();
  printf("thread 2\n");
#if 1
  for (int i = 0; i < 525; i++) {
    printf("a ");
  }
  printf("\n");
#endif
  mythread_exit();
}

int main() {
#if 1
  mythread_init();
  set_quantum_size(100);
  int id = mythread_create("t1", testthread, 16384);
  if (id == -1) {
    exit(1);
  }

#if 1
  id = mythread_create("t2", testthread2, 16384);
  if (id == -1) {
    exit(1);
  }
#endif
  mythread_state();

  printQueue();
  printf("\n\n");

  runthreads();
  mythread_state();
#endif
}
