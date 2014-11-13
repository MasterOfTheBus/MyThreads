/*
 * Sidney Ng
 * 260507001
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
//#include <semaphore.h>
#include <unistd.h>
#include "mythread.hpp"

#define TIMEOUT 5
#define NUM_EXECUTION 5

int buffsize;
int clients;
int printers;

/*sem_t*/ int mutex;
/*sem_t*/ int full;
/*sem_t*/ int empty;

#define err_exit(no, msg) errno = no; perror(msg); exit(EXIT_FAILURE);

//============== Buffer Declaration =================

typedef struct {
  int *slots;
  int length;
  int head;
  int tail;
} CircularBuffer;

CircularBuffer newCircularBuffer() {
  CircularBuffer cb;
  cb.length = 0;
  cb.head = (buffsize == 1) ? 0 : 1;
  cb.tail = 0;
  cb.slots = (int*)malloc(sizeof(int) * buffsize);
  return cb;
}

void enqueue(CircularBuffer *cb, int num) {
  if (cb->tail == buffsize-1) {
	cb->tail = 0;
  } else {
	cb->tail++;
  }
  cb->slots[cb->tail] = num;
  cb->length++;
}

int dequeue(CircularBuffer *cb) {
  int dq;
  dq = cb->slots[cb->head];
  cb->slots[cb->head] = -1;
  if (cb->head == buffsize-1) {
    cb->head = 0;
  } else {
    cb->head++;
  }
  cb->length--;
  return (dq);
}

//================== End Buffer Declaration ==============

typedef struct {
  CircularBuffer *cb;
  int tid;
} targs;

static void *producer(void *arg) {
    targs *threadArgs = (targs*)arg;
    CircularBuffer *cb = threadArgs->cb;
    int tid = (int)threadArgs->tid;
    int pages;
    srand (time(NULL));
    free(arg);

    /*
     * The count here could be replace with while(1) if the program is meant
     * to infinitely loop
     */
    int count = 0;
    while (count < NUM_EXECUTION) {
      pages = rand() % 10 + 1;

      sem_trywait(&empty);
      sem_wait(&mutex);
      while (cb->length == buffsize) {
	    printf("Client %d has %d pages to print, buffer full, sleeping\n", tid, pages);
        sem_post(&mutex);
	    sem_wait(&empty);
	    printf("Client %d waking up, putting request in Buffer[%d]\n", tid, cb->tail);
        sem_wait(&mutex);
      }
      enqueue(cb, pages);
      printf("Client %d has %d pages to print, putting request in Buffer[%d]\n",
             tid, pages, cb->tail);
      sem_post(&mutex);
      sem_post(&full);

      count++;
      sleep(TIMEOUT);
    }
}

static void *consumer(void *arg) {
    targs *threadArgs = (targs*)arg;
    CircularBuffer *cb = threadArgs->cb;
    int tid = (int)threadArgs->tid;
    free (arg);

    while (1) {
      sem_trywait(&full);
      sem_wait(&mutex);
      while (cb->length == 0) {
	    printf("No requests in buffer, Printer %d going to sleep\n", tid);
        sem_post(&mutex);
	    sem_wait(&full);
        sem_wait(&mutex);
      }
      int head = cb->head;
      int pages = dequeue(cb);
      if (pages < 0) {
        printf("Error: pages to print less than 0\n");
        continue;
      }
      printf("Printer %d printing %d pages from Buffer[%d]\n", tid, pages, head);
      sleep(pages);
      printf("Printer %d finished printing %d pages from Buffer[%d]\n", tid, pages, head);
      sem_post(&mutex);
      sem_post(&empty);
    }
}

