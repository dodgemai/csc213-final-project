#include "mcache.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#define FIB_DELAY 25

size_t fibonacci_bad(int n) {
  if(n == 0 || n == 1) {
    return n;
  } else {
    return fibonacci_bad(n - 1) + fibonacci_bad(n - 2);
  }
}

int main(int argc, char** argv) {
  if(argc != 2) {
    fprintf(stderr, "usage: %s <num-to-test>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  int num_vals = atoi(argv[1]);
  if(num_vals <= 0) { return 2; }

  mcache_init("localhost");

  //printf("Testing with int values...\n");
  for(int i = 0; i <= num_vals; i++) {
    char key[25];
    sprintf(key, "key%d", i);
    mcache_add(key, &(i), sizeof(i));
    //printf("%s: %d\n", key, *(int*)mcache_get(key));
    //printf("Got value %d from key %s.\n", *(int*)mcache_get(key), key);
    assert(*((int*)mcache_get(key)) == i);
  }

  //printf("Testing for proper eviction...\n");
  int successful_gets = 0;
  for(int i = 0; i <= num_vals; i++) {
    char key[25];
    sprintf(key, "key%d", i);
    int* val;
    if((val = mcache_get(key)) != NULL) {
      assert(*val == i);
      //printf("Successfully got %d\n", i);
      successful_gets++;
    } else {
      /* cache miss!!! artificial delay */
      fibonacci_bad(FIB_DELAY);
    }
  }
  //printf("Successful gets: %d\n", successful_gets);
  assert((successful_gets == MCACHE_MAX_ALLOCATION / 4)
          || successful_gets == num_vals + 1);

  //printf("All tests successful.\n");

  mcache_exit();
}
