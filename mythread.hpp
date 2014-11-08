#include <ucontext.h>
#include <signal.h>

#define MAX_THREADS 100
#define MAX_SEMAPHORES 100
#define THREAD_NAME_LEN 128

#define error_msg(msg) \
  { perror(msg); exit(EXIT_FAILURE); }

namespace mythreads {
  typedef enum {
    RUNNABLE,
    RUNNING,
    BLOCKED,
    EXIT
  } ThreadState;

  typedef struct _mythread_control_block {
    ucontext_t context;
    //char thread_name[THREAD_NAME_LEN];
    char* thread_name;
    int thread_id;
    ThreadState state;
  } mythread_control_block;

  typedef struct _semaphore_control_block {
    int id;
    int count;
  } semaphore_control_block;

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
}
