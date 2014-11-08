#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <map>
using namespace std;

#define MAIN_THREAD 0
#define RR_SCHED 1

#include "mythread.hpp"
namespace mythreads {
  int numThreads;
  int numSemaphores;
  map<int, mythread_control_block> threadTable;
  map<int, semaphore_control_block> semaphoreTable;
  queue<mythread_control_block> runqueue;
  queue<mythread_control_block> waitqueue;

  // initialize the global variables and data structures
  int mythread_init() {
    // save the context for the main thread and add it to the table of threads
    mythread_control_block block;
    block.thread_id = MAIN_THREAD;
    block.thread_name = "main";
    block.state = RUNNABLE;
    ucontext_t uc;
    block.context = uc;
    threadTable[MAIN_THREAD] = block;

    // initialize the round robin scheduler but don't run it yet
    block.thread_id = RR_SCHED;
    block.thread_name = "scheduler";
    block.state = RUNNABLE;
    ucontext_t uct;
    if (getcontext(&uct) == -1) {
      error_msg("getcontext");
    }
    int func_stack[SIGSTKSZ];
    uct.uc_stack.ss_sp = func_stack;
    uct.uc_stack.ss_size = SIGSTKSZ;
    uct.uc_link = &threadTable[MAIN_THREAD].context;
    makecontext(&uct, rrScheduler, 0);
    block.context = uct;
    threadTable[RR_SCHED] = block;

    numThreads = RR_SCHED;
    numSemaphores = 0;
  }

  // TODO, variable list of args
  int mythread_create(char *threadname, void (*threadfunc)(), int stacksize) {
    mythread_control_block controlBlock;
    controlBlock.thread_name = threadname;
    numThreads++;
    controlBlock.thread_id = numThreads;
    controlBlock.state = RUNNABLE;

    ucontext_t uct;
    if (getcontext(&uct) == -1) {
      error_msg("getcontext");
    }
    int func_stack[stacksize];
    uct.uc_stack.ss_sp = func_stack;
    uct.uc_stack.ss_size = stacksize;
    uct.uc_link = &threadTable[RR_SCHED].context; // TODO: figure out if the link should be to scheduler
    makecontext(&uct, threadfunc, 0); // TODO: pass in a variadic list

    controlBlock.context = uct;

    threadTable[controlBlock.thread_id] = controlBlock;
    runqueue.push(controlBlock);

    return (controlBlock.thread_id);
  }

  void mythread_exit() {
    mythread_control_block block = runqueue.front();
    runqueue.pop();

    threadTable[block.thread_id].state = EXIT;
  }

  void runthreads() {
    // start the scheduler

    // swap context from main to a thread
    ucontext_t uct = runqueue.front().context;
    runqueue.pop();
    if (swapcontext(&threadTable[MAIN_THREAD].context, &uct) == -1) {
      error_msg("swapcontext");
    }
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

  void rrScheduler() {

  }

} // namespace mythreads
