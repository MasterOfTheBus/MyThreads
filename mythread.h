#include <ucontext.h>
#include <slack/list.h>

typedef struct _mythread_control_block {
  ucontext_t context;
  char thread_name[THREAD_NAME_LEN];
  int thread_id;
} mythread_control_block;

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
