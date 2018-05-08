#include "mcache.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

int main(void) {
  mcache_init("localhost");

  printf("Testing with int values...\n");
  for(int i = 0; i <= 9999; i++) {
    char key[25];
    sprintf(key, "key%d", i);
    mcache_set(key, &(i), sizeof(i));
    //printf("%s: %d\n", key, *(int*)mcache_get(key));
    assert(*((int*)mcache_get(key)) == i);
  }

  printf("Testing for proper eviction...\n");
  int successful_gets = 0;
  for(int i = 0; i <= 9999; i++) {
    char key[25];
    sprintf(key, "key%d", i);
    int* val;
    if((val = mcache_get(key)) != NULL) {
      assert(*val == i);
      //printf("Successfully got %d\n", i);
      successful_gets++;
    }
  }

  assert(successful_gets == MCACHE_MAX_ALLOCATION / 4);
  //printf("Successful gets: %d\n", successful_gets);
  printf("All tests successful.\n");

  mcache_exit();
}
