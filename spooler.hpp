/*
 * Sidney Ng
 * 260507001
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <semaphore.h>
#include <unistd.h>

#define TIMEOUT 5
#define NUM_EXECUTION 5

int buffsize;
int clients;
int printers;
/*
sem_t mutex;
sem_t full;
sem_t empty;
*/
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

