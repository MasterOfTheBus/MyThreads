#include <ucontext.h>
#include <signal.h>
#include <queue>

#define DEFAULT_SEC 0
#define DEFAULT_USEC 1000 // microseconds

#define error_msg(msg) \
  { perror(msg); exit(EXIT_FAILURE); }

namespace mythreads {
  typedef enum {
    RUNNABLE,
    RUNNING,
    WAIT,
    BLOCKED,
    EXIT
  } ThreadState;

  typedef struct _mythread_control_block {
    ucontext_t* context;
    char* thread_name;
    int thread_id;
    ThreadState state;
    // time on cpu
  } mythread_control_block;

  typedef struct _semaphore_control_block {
    int id;
    int value;
    int count;
    std::queue<int> waiting_threads;
  } semaphore_control_block;

  int mythread_init();
  int mythread_create(char *threadname, void (*threadfunc)(), int stacksize);
  void mythread_exit();
  void runthreads();
  void set_quantum_size(int quantum);
  int create_semaphore(int value);
  void semaphore_wait(int semaphore);
  int semaphore_getvalue(int semaphore);
  void semaphore_signal(int semaphore);
  void destroy_semaphore(int semaphore);
  char* stateString(ThreadState state);
  void mythread_state();
  void rrScheduler();
  void printQueue();
  void printSem();
}
