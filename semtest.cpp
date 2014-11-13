#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mythread.hpp"
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace mythreads;
using namespace std;

//int file = "test.txt";
int mutex;

void reader() {
    int count = 0;
    
    while (count < 15) {
      semaphore_wait(mutex);
      string line;
      ifstream myfile ("test.txt");
      if (myfile.is_open()) {
	while (getline(myfile, line)) {
	    cout << line << '\n';
	}
	myfile.close();
      } else {
	cout << "Couldn't open file\n";
      }
      semaphore_signal(mutex);
      count++;
    }
}

void writer() {
    int count = 0;
    srand(time(NULL));
    string writeThis[] = {"As the world turns...\n", "Information is ... Protection\n"};
    while (count < 15) {
      int index = rand() % 2 + 1;
      semaphore_wait (mutex);
      ofstream myfile;
      myfile.open ("test.txt");
      myfile << writeThis[index];
      myfile.close();
      semaphore_signal(mutex);
      count++;
    }
}

int main() {
  mythread_init();
  set_quantum_size(100);
  int id = mythread_create("reader", reader, 16384);
  if (id == -1) {
    exit(1);
  }

  id = mythread_create("writer", writer, 16384);
  if (id == -1) {
    exit(1);
  }

  mutex = create_semaphore(1);

  mythread_state();

  printQueue();
  printf("\n\n");

  runthreads();
  mythread_state();
}
