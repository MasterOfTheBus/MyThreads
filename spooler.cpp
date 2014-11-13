/*
 * Sidney Ng
 * 260507001
 */

#include <string.h>
#include <pthread.h>
#include "spooler.hpp"
#include <ctype.h>

#include "mythread.hpp"
using namespace mythreads;

CircularBuffer buff;
int numClients, numPrinters;
int mutex, full, empty; 

void producer() {
    int tid = numClients;
    int pages;
    srand (time(NULL));

    /*
     * The count here could be replace with while(1) if the program is meant
     * to infinitely loop
     */
    int count = 0;
    while (count < NUM_EXECUTION) {
      pages = rand() % 10 + 1;
printf("producing...\n");
#if 0
      if (semaphore_getvalue(empty) <= 0) {
        semaphore_wait(empty); // trywait
      }
printf("past empty\n");
      semaphore_wait(mutex);
printf("past mutex for producer\n");
      while (buff.length == buffsize) {
	printf("Client %d has %d pages to print, buffer full, sleeping\n", tid, pages);
	semaphore_signal(mutex);
	semaphore_wait(empty);
	printf("Client %d waking up, putting request in Buffer[%d]\n", tid, buff.tail);
	semaphore_wait(mutex);
      }
#endif
      semaphore_wait(empty);
      semaphore_wait(mutex);
      enqueue(&buff, pages);
      printf("Client %d has %d pages to print, putting request in Buffer[%d]\n",
             tid, pages, buff.tail);
#if 1
      semaphore_signal(mutex);
      semaphore_signal(full);
#endif
      count++;
      sleep(TIMEOUT);
    }
    mythread_exit();
}

void consumer() {
    int tid = numPrinters;;
int toPrint = 0;
    while (1) {
printf("consuming...\n");
#if 0
      //if (semaphore_getvalue(full) <= 0) {
        semaphore_wait(full); // trywait
      //}
printf("past full\n");
      semaphore_wait(mutex);
printf("past mutex for consumer\n");
      while (buff.length == 0) {
	printf("No requests in buffer, Printer %d going to sleep\n", tid);
        semaphore_signal(mutex);
	semaphore_wait(full);
        semaphore_wait(mutex);
      }
#endif
      semaphore_wait(full);
      semaphore_wait(mutex);
      int head = buff.head;
      int pages = dequeue(&buff);
      if (pages < 0) {
        printf("Error: pages to print less than 0\n");
	if (toPrint > 10) {
	    return;
	}
	toPrint++;
        continue;
      }
      printf("Printer %d printing %d pages from Buffer[%d]\n", tid, pages, head);
      sleep(pages);
      printf("Printer %d finished printing %d pages from Buffer[%d]\n", tid, pages, head);
#if 1
      semaphore_signal(mutex);
      semaphore_signal(empty);
#endif
    }
}

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
  numClients = 0;
  numPrinters = 0;

  mutex = create_semaphore(1);
  full = create_semaphore(0);
  empty = create_semaphore(buffsize);
  printf("mutex: %d, full: %d, empty: %d\n", mutex, full, empty);

  // create the circular buffer
  buff = newCircularBuffer();

  int client[clients];
  int printer[printers];

  // Create the clients
  i = 0;
  for (; i < clients; i++) {
    char name[10];
    sprintf(name, "client%d", i);
    s = mythread_create(name, producer, 16384);
    printf("s: %d\n", s);
    if (s == -1) {
      err_exit(s, "mythread_create");
    }
    numClients++;
  }

  // Create the printers
  i = 0;
  for (; i < printers; i++) {
    char name[10];
    sprintf(name, "printer%d", i);
    s = mythread_create(name, consumer, 16384);
    printf("s: %d\n", s);
    if (s == -1) {
      err_exit(s, "mythread_create");
    }
    numPrinters++;
  }

  printf("printing sems\n");
  printSem();

  mythread_state();
  printQueue();
  printf("\n\n");

  set_quantum_size(100);

  runthreads();

  mythread_state();
  printQueue();
  free(buff.slots);

  exit(0);
}
