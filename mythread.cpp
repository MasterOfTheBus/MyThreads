#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <map>
using namespace std;

#define MAIN_THREAD 0
#define RR_SCHED 1
#define MAX_STACK_SIZE 16384

#include "mythread.hpp"
namespace mythreads {
  int numThreads;
  int numSemaphores;
  map<int, ucontext_t*> contextTable;
  map<int, mythread_control_block> threadTable;
  map<int, semaphore_control_block> semaphoreTable;
  queue<mythread_control_block> runqueue;
  queue<mythread_control_block> waitqueue;
  ucontext_t uctx_main, uctx_sched;

  // initialize the global variables and data structures
  int mythread_init() {
    // save the context for the main thread and add it to the table of threads
    mythread_control_block block;
    block.thread_id = MAIN_THREAD;
    block.thread_name = "main";
    block.state = RUNNABLE;
    block.context = &uctx_main;
    threadTable[MAIN_THREAD] = block;

    // initialize the round robin scheduler but don't run it yet
    block.thread_id = RR_SCHED;
    block.thread_name = "scheduler";
    block.state = RUNNABLE;
    if (getcontext(&uctx_sched) == -1) {
      error_msg("getcontext");
    }
    uctx_sched.uc_stack.ss_sp = malloc(MAX_STACK_SIZE);
    uctx_sched.uc_stack.ss_size = MAX_STACK_SIZE;
    uctx_sched.uc_link = &uctx_main;
    makecontext(&uctx_sched, rrScheduler, 0);
    block.context = &uctx_sched;
    threadTable[RR_SCHED] = block;

    numThreads = RR_SCHED;
    numSemaphores = 0;
  }

  // TODO, variable list of args
  int mythread_create(char *threadname, void (*threadfunc)(), int stacksize) {
    if (stacksize > MAX_STACK_SIZE) {
      printf("stack size exceeds limit\n");
      return (-1);
    }

    mythread_control_block controlBlock;
    controlBlock.thread_name = threadname;
    numThreads++;
    controlBlock.thread_id = numThreads;
    controlBlock.state = RUNNABLE;

    ucontext_t *uct = (ucontext_t*)malloc(sizeof(ucontext_t));
    if (getcontext(uct) == -1) {
      error_msg("getcontext");
    }
    uct->uc_stack.ss_sp = malloc(stacksize);
    uct->uc_stack.ss_size = stacksize;
    uct->uc_link = &uctx_sched; // TODO: figure out if the link should be to scheduler
    makecontext(uct, threadfunc, 0); // TODO: pass in a variadic list
    contextTable[controlBlock.thread_id] = uct;

    controlBlock.context = uct;
    //block = controlBlock;
    threadTable[controlBlock.thread_id] = controlBlock;
    runqueue.push(controlBlock);

    return (controlBlock.thread_id);
  }

  void mythread_exit() {
    mythread_control_block block = runqueue.front();
    runqueue.pop();

    // free the thread stack here?
    free(block.context->uc_stack.ss_sp);
    threadTable[block.thread_id].state = EXIT;

    // free the context
    free(contextTable[block.thread_id]);
  }

  void runthreads() {
    // start the scheduler

    // swap context from main to a thread
    mythread_control_block block = runqueue.front();
    ucontext_t *uct = contextTable[block.thread_id];
    block.state = RUNNING;
    runqueue.pop();

    if (threadTable[MAIN_THREAD].context == NULL) {
	printf("null main\n");
    }
    if (uct == NULL) {
	printf("null function context\n");
    }

    if (swapcontext(&uctx_main, uct) == -1) {
      error_msg("swapcontext");
    }
#if 0
    if (swapcontext(threadTable[MAIN_THREAD].context,
                    threadTable[RR_SCHED].context) == -1) {
      error_msg("swapcontext");
    }
#endif
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

  char* stateString(ThreadState state) {
    char *ret;
    switch (state) {
    case RUNNABLE:
      ret = "RUNNABLE";
      break;
    case RUNNING:
      ret = "RUNNING";
      break;
    case BLOCKED:
      ret = "BLOCKED";
      break;
    case EXIT:
      ret = "EXIT";
      break;
    default:
      break;
    }
    return (ret);
  }

  void mythread_state() {
    mythread_control_block block = threadTable[RR_SCHED];
    map<int, mythread_control_block>::iterator it = threadTable.begin();
    printf("Name\tState\tTime on CPU\n");
    while (it != threadTable.end()) {
      printf("%s\t%s\t%d\n", it->second.thread_name,
             stateString(it->second.state), 0);
      it++;
    }
  }

  void rrScheduler() {
    printf("returning\n");
  }

} // namespace mythreads
