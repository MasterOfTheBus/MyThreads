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

//ucontext_t uctx_main, func_c, func_d;

int main() {
#if 0
    if (getcontext(&func_d) == -1) {
	printf("error\n");
	exit(1);
    }
    func_d.uc_stack.ss_sp = malloc(8192);
    func_d.uc_stack.ss_size = 8192;
    func_d.uc_link = &uctx_main;
    makecontext(&func_d, testthread2, 0);

    if (getcontext(&func_c) == -1) {
	printf("error\n");
	exit(1);
    }
    func_c.uc_stack.ss_sp = malloc(8192);
    func_c.uc_stack.ss_size = 8192;
    func_c.uc_link = &func_d;
    makecontext(&func_c, testthread, 0);

    printf("main swapping context to testthread\n");
    if (swapcontext(&uctx_main, &func_c) == -1) {
	printf("error swap\n");
	exit(1);
    }
    printf("main exiting success\n");
    exit(0);
#endif
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
