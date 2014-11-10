#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mythread.hpp"

using namespace mythreads;

void testthread() {
  printf("a\n");
  //mythread_exit();
}

void testthread2(char *str) {
  printf("%s\n", str);
}

ucontext_t uctx_main, func_c;
char func1_stack[16384];

int main() {
    //char func1_stack[16384];
//    ucontext_t func_c;
#if 0
    if (getcontext(&func_c) == -1) {
	printf("error\n");
	exit(1);
    }
    func_c.uc_stack.ss_sp = func1_stack;//malloc(8192);
    func_c.uc_stack.ss_size = sizeof(func1_stack);//8192;
    func_c.uc_link = &uctx_main;
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
  int thread_ids[2];
  int id = mythread_create("t1", testthread, 16384);
  if (id == -1) {
    exit(1);
  }

#if 0
  id = mythread_create("t2", testthread2, SIGSTKSZ);
  if (id != -1) {
    thread_ids[1] = id;
  }
#endif
  mythread_state();

  runthreads();

  mythread_state();
#endif
}
