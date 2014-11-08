#include <ucontext.h>
#include <signal.h>
#include <slack/std.h>
#include <slack/lib.h>
#include <slack/list.h>
#include <slack/map.h>

#define MAX_THREADS 100
#define MAX_SEMAPHORES 100
#define THREAD_NAME_LEN 128

#define error_msg(msg) \
  { perror(msg); exit(EXIT_FAILURE); }

typedef enum {
  RUNNABLE,
  RUNNING,
  BLOCKED,
  EXIT
} ThreadState;

typedef struct _mythread_control_block {
  ucontext_t context;
  char thread_name[THREAD_NAME_LEN];
  int thread_id;
  ThreadState state;
} mythread_control_block;

typedef struct _semaphore_control_block {
  count;
} semaphore_control_block;

int numThreads;
int numSemaphores;
int threadTable[MAX_THREADS];
//List *threadTable;
int semaphoreTable[MAX_SEMAPHORES];
List *runqueue;
List *waitqueue;

int mythread_init();
int mythread_create(char *threadname, void (*threadfunc)(), int stacksize);
void mythread_exit();
void runthreads();
void set_quantum_size(int quantum);
int create_semaphore(int value);
void semaphore_wait(int semaphore);
void semaphore_signal(int semaphore);
void destroy_semaphore(int semaphore);
void mythread_state();
