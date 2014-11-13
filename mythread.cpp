#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <queue>
#include <map>
using namespace std;

#define MAIN_THREAD 0
#define RR_SCHED 1
#define MAX_STACK_SIZE 16384

#include "mythread.hpp"
namespace mythreads {
  int numThreads, numSemaphores, currentThread;
  int quantum_usec, quantum_sec;
  map<int, ucontext_t*> contextTable;
  map<int, queue<int>* > waitQueueTable;
  map<int, mythread_control_block> threadTable;
  map<int, semaphore_control_block> semaphoreTable;
  queue<int> runqueue;
  queue<int> waitqueue;
  ucontext_t uctx_main, uctx_sched;
  struct itimerval tval;
  sigset_t sset, oldset;

  void pauseTimer() {
#if 1
    struct itimerval pauseVal = { 0 };
    setitimer(ITIMER_REAL, &pauseVal, &tval);
#endif
#if 0
    sigprocmask(SIG_BLOCK, &sset, &oldset);
#endif
  }

  void resumeTimer() {
#if 1
    setitimer(ITIMER_REAL, &tval, 0);
#endif
#if 0
    sigprocmask(SIG_SETMASK, &oldset, 0);
#endif
  }

  /*
   * Round Robin Scheduler invoked everytime sigalrm passed
   */
  void rrScheduler() {
    while (!runqueue.empty()) {
      // pause the timer to make sure this section won't be interrupted
      pauseTimer();
      /*
      printf("==========searching for active thread\n");
      printQueue();
      */
      // check if a thread has exited and remove it from the queue
      int id = runqueue.front();
      while ((threadTable[id].state == EXIT) && (!runqueue.empty())) {
	runqueue.pop();
	id = runqueue.front();
      }
      //printQueue();
      if (runqueue.empty()) {
        currentThread == MAIN_THREAD;
	return;
      }
      // the current thread will be on the top, so pop and then push it before taking the next
      runqueue.pop();
      runqueue.push(id);
      id = runqueue.front();
      //printf("==========found an active thread\n");
      ucontext_t *uct = contextTable[id];
      threadTable[id].state = RUNNING;
#if 0
      runqueue.pop();
      runqueue.push(currentThread);
#endif
      threadTable[currentThread].state == RUNNABLE;
      currentThread = id;

      //printf("id: %d\n\n===============swapping context\n", id);
      resumeTimer();
      if (swapcontext(&uctx_sched, uct) == -1) {
        error_msg("swapcontext");
      }
    }
    return;
  }

  // context switcher that the timer will call to save current context and switch to scheduler
  void swapToSched(int signum) {
    // get the current context, swap from that to the scheduler
    printf("swapping contexts=========================\n");
    if (swapcontext(contextTable[currentThread], &uctx_sched) == -1) {
      error_msg("swapcontext");
    }
  }

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

#if 0
    // start the timer
    struct sigaction nact;
    nact.sa_handler = swapToSched;//rrScheduler_sig;
    nact.sa_flags = 0;
    sigaction (SIGALRM, &nact, 0);
#endif
#if 1
    sigset(SIGALRM, swapToSched);
#endif
    sigemptyset(&sset);
    sigemptyset(&oldset);
    sigaddset(&sset, SIGALRM);
    tval.it_interval.tv_sec = quantum_sec;
    tval.it_interval.tv_usec = quantum_usec;
    tval.it_value.tv_sec = quantum_sec;
    tval.it_value.tv_usec = quantum_usec;

    numThreads = RR_SCHED;
    numSemaphores = 0;
    quantum_sec = DEFAULT_SEC;
    quantum_usec = DEFAULT_USEC;
    currentThread = MAIN_THREAD;
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
    uct->uc_link = &uctx_sched;
    makecontext(uct, threadfunc, 0);
    contextTable[controlBlock.thread_id] = uct;

    controlBlock.context = uct;
    threadTable[controlBlock.thread_id] = controlBlock;
    runqueue.push(controlBlock.thread_id);
    //printf("size: %d\n", runqueue.size());
    //printf("back: %d\n", runqueue.back());

    return (controlBlock.thread_id);
  }

  void mythread_exit() {
    if (runqueue.empty()) {
      error_msg("no scheduled threads");
    }
    // free the thread stack here?
    //    free(block.context->uc_stack.ss_sp);
    printf("thread exiting\n\n");
    threadTable[currentThread].state = EXIT;
    // free the context
    free(contextTable[currentThread]);
    map<int, ucontext_t*>::iterator it = contextTable.find(currentThread);
    //ucontext_t *toFree = contextTable[id];
    contextTable.erase(it);
    //free(toFree);
  }

  void runthreads() {
    // swap context from main to a thread
    int id = runqueue.front();
    ucontext_t *uct = contextTable[id];
    threadTable[id].state = RUNNING;
    //runqueue.pop();
    //runqueue.push(id);
    currentThread = id;
    
    setitimer(ITIMER_REAL, &tval, 0);

    if (swapcontext(&uctx_main, uct) == -1) {
      error_msg("swapcontext");
    }
  }

  /*
   * quantum is in microseconds
   */
  void set_quantum_size(int quantum) {
    if (quantum <= MIN_INTERVAL) {
      printf("Quantum size must be greater than 0\n");
      exit(1);
    }
    quantum_sec = (quantum > 100000) ? quantum / 100000 : 0;
    quantum_usec =(quantum > 100000) ? quantum % 100000 : quantum;
    tval.it_interval.tv_sec = quantum_sec;
    tval.it_interval.tv_usec = quantum_usec;
    tval.it_value.tv_sec = quantum_sec;
    tval.it_value.tv_usec = quantum_usec;
  }

  int create_semaphore(int value) {
    semaphore_control_block sem_block;
    sem_block.id = numSemaphores;
    sem_block.count = value;
    sem_block.value = value;
    queue<int> waitQ;
    sem_block.waiting_threads = waitQ;
    waitQueueTable[sem_block.id] = &waitQ;
    semaphoreTable[sem_block.id] = sem_block;
    numSemaphores++;
    return (sem_block.id);
  }

  void semaphore_wait(int semaphore) {
    pauseTimer();
    semaphoreTable[semaphore].count--;
    //printf("count value: %d\n", semaphoreTable[semaphore].count);
    if (semaphoreTable[semaphore].count < 0) {
      semaphoreTable[semaphore].waiting_threads.push(currentThread);
      threadTable[currentThread].state = WAIT;
      // the current running thread will be on the front of the runqueue, so just pop it
      runqueue.pop();
      // swap the context to the scheduler
      resumeTimer();
      if (swapcontext(contextTable[currentThread], &uctx_sched) == -1) {
	error_msg("swapcontext");
      }
    } else {
      resumeTimer();
    }
  }

  void semaphore_signal(int semaphore) {
    pauseTimer();
    semaphoreTable[semaphore].count++;
    //printf("count value: %d\n", semaphoreTable[semaphore].count);
    if (semaphoreTable[semaphore].count <= 0) {
      int tid = semaphoreTable[semaphore].waiting_threads.front();
      runqueue.push(tid);
      threadTable[tid].state = RUNNABLE;
      semaphoreTable[semaphore].waiting_threads.pop();
    }
    resumeTimer();
  }

  void destroy_semaphore(int semaphore) {
    pauseTimer();
    if (!semaphoreTable[semaphore].waiting_threads.empty()) {
      printf("Threads waiting on semaphore %d, cannot destroy\n", semaphore);
      return;
    }
    if (semaphoreTable[semaphore].count != semaphoreTable[semaphore].value) {
      printf("Semaphore count and semaphore value mismatch"
	     " during destroy for semaphore %d\n", semaphore);
    }
    map<int, semaphore_control_block>::iterator it = semaphoreTable.find(semaphore);
    semaphoreTable.erase(it);
    resumeTimer();
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
    printf("Id\tName\t\tState\tTime on CPU\n");
    while (it != threadTable.end()) {
      printf("%d\t%s\t\t%s\t%d\n", it->second.thread_id,
             it->second.thread_name, stateString(it->second.state), 0);
      it++;
    }
  }

  void printQueue() {
    if (!runqueue.empty()) {
      printf("front: %d\n", runqueue.front());
      printf("back: %d\n", runqueue.back());
      printf("size: %d\n", runqueue.size());
    } else {
      printf("queue empty\n");
    }
    printf("currentThread: %d\n", currentThread);
  }

  int semaphore_getvalue(int semaphore) {
    return semaphoreTable[semaphore].count;
  }

  void printSem() {
    if (semaphoreTable.empty()) {
      printf("map empty\n");
    } else {
      map<int, semaphore_control_block>::iterator it = semaphoreTable.begin();
      while (it != semaphoreTable.end()) {
        printf("id: %d, value: %d, count: %d\n", it->second.id, it->second.value, it->second.count);
	it++;
      }
    }
  }

} // namespace mythreads
