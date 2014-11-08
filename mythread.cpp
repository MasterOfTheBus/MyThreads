#include <stdio.h>
#include <stdlib.h>

// initialize the global variables and data structures
int mythread_init() {
  // initialize the queues
  runqueue = list_create(NULL);
  waitqueue = list_create(NULL);
  
  mythread_control_block mainBlock;
  ucontext_t uc;
  if (getContext(&uc) == -1) {
    error_msg("getcontext");
  }
  mainBlock.context = uc;

  // save the context for the main thread and add it to the table of threads
  numThreads = 0;
  numSemaphores = 0;
}

int mythread_create(char *threadname, void (*threadfunc)(), int stacksize) {
  mythread_control_block controlBlock;
  controlBlock.thread_name = threadname;
  
  stack_t ucp_stack;
}

void mythread_exit() {

}

void runthreads() {

}

void set_quantum_size(int quantum) {

}

int create_semaphore(int value) {

}

void semaphore_wait(int semaphore) {

}

void semaphore_signal(int semaphore) {

}

void destroy_semaphore(int semaphore) {

}

void mythread_state() {

}
