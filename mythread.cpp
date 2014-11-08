#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <map>
using namespace std;

#include "mythread.hpp"
namespace mythreads {
   int numThreads;
   int numSemaphores;
   map<int, mythread_control_block> threadTable;
   map<int, semaphore_control_block> semaphoreTable;
   queue<int> runqueue;
   queue<int> waitqueue;

  // initialize the global variables and data structures
  int mythread_init() {
    mythread_control_block mainBlock;
    ucontext_t uc;
    if (getcontext(&uc) == -1) {
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

} // namespace mythreads
