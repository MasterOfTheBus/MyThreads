/*
 * Sidney Ng
 * 260507001
 */

#include <string.h>
//#include <pthread.h>
//#include "mythread.hpp"
#include "spooler.h"

int main(int argc, char *argv[])
{
  // get the option values
  if (argc != 4) {
    printf("usage: spooler -c[clients] -p[printers] -b[buffer size]\n");
    exit(1);
  }

  int i = 1;
  for (; i < argc; i++) {
    if (strstr(argv[i], "-c") != NULL) {
      if (isdigit(argv[i][2])) {
        clients = atoi(&argv[i][2]);
      } else {
        printf("Number of clients must be an integer value\n");
        exit(1);
      }
    } else if (strstr(argv[i], "-p") != NULL) {
      if (isdigit(argv[i][2])) {
        printers = atoi(&argv[i][2]);
      } else {
        printf("Number of printers must be an integer value\n");
        exit(1);
      }
    } else if (strstr(argv[i], "-b") != NULL) {
      if (isdigit(argv[i][2])) {
        buffsize = atoi(&argv[i][2]);
      } else {
        printf("Buffer size must be an integer value\n");
        exit(1);
      }
    } else {
      printf("usage: spooler -c[clients] -p[printers] -b[buffer size]\n");
      exit(1);
    }
  }

  if (clients <= 0) {
    printf("Number of clients must be greater than 0\n");
    exit(1);
  }
  if (printers <= 0) {
    printf("Number of printers must be greater than 0\n");
    exit(1);
  }
  if (buffsize <= 0) {
    printf("Buffer size must be greater than 0\n");
    exit(1);
  }

  void *ret;
  int s;

  mythread_init();
  set_quantum_size(150);
  mutex = create_semaphore(1);
  full = create_semaphore(0);
  empty = create_semaphore(buffsize);
/*
  sem_init(&mutex, 0, 1);
  sem_init(&full, 0, 0);
  sem_init(&empty, 0, buffsize);
*/
  // create the circular buffer
  CircularBuffer buff = newCircularBuffer();

  /*pthread_t*/ int client[clients];
  /*pthread_t*/ int printer[printers];

  // Create the clients
  i = 0;
  for (; i < clients; i++) {
    targs *thread_args = malloc(sizeof(targs));
    thread_args->cb = &buff;
    thread_args->tid = i;
    s = pthread_create(&client[i], NULL, producer, thread_args);
    if (s != 0) {
      err_exit(s, "pthread_create");
    }
    char name[10];
    sprintf(name, "client%d", i);
    client[i] = mythread_create(name, producer
  }

  // Create the printers
  i = 0;
  for (; i < printers; i++) {
    targs *thread_args = malloc(sizeof(targs));
    thread_args->cb = &buff;
    thread_args->tid = i;
    s = pthread_create(&printer[i], NULL, consumer, thread_args);
    if (s != 0) {
      err_exit(s, "pthread_create");
    }
  }

  i = 0;
  for (; i < clients; i++) {
    s = pthread_join(client[i], &ret);
    if (s != 0) {
      err_exit(s, "pthread_join");
    }
    printf("Joined with client %d, returning\n", i);
  }

  i = 0;
  for (; i < printers; i++) {
    s = pthread_cancel(printer[i]);
    if (s != 0) {
      err_exit(s, "pthread_cancel");
    }
    s = pthread_join(printer[i], &ret);
    if (s != 0) {
      err_exit(s, "pthread_join");
    }
    printf("Joined with printer %d, returning\n", i);
  }

  free(buff.slots);

  exit(0);
}
