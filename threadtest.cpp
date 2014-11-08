#include <stdio.h>
#include <stdlib.h>
#include "mythread.hpp"

using namespace mythreads;

void testthread() {
  printf("a");
}

int main() {
  mythread_init();

}
