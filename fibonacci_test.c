#include "mcache.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

//update key with new n value -- note: key should already be null terminated
void update_key(int n, char* key) {
  sprintf(key, "%d", n);
  /*this whole key thing doesn't work if n is a power of two, so, if it's
   * a power of two then just make the key something larger, that's not
   * a power of two
   * this will break if you try to make it break, but just don't do that, this
   * is a short testing thing only!
   */
   /*
  if(n && !(n & (n - 1))) {
    n *= 47621;
  }
  memcpy(key, &n, sizeof(n));
  */
}

size_t fibonacci(int n) {
  char key1[25];
  char key2[25];
  char key3[25];

  //check to see if n value has already been cached...
  update_key(n, key1);
  size_t* v = mcache_get(key1);

  //if cached, free val and return it
  if(v != NULL) {
    size_t tmp = *v;
    free(v);
    return tmp;
  }

  int f0 = 0, f1 = 1;
  update_key(f0, key2);
  mcache_add(key2, &(size_t) { 0 }, sizeof(size_t));

  update_key(f1, key3);
  mcache_add(key3, &(size_t) { 1 }, sizeof(size_t));

  for(int i = 2; i <= n; i++) {
    //update keys
    update_key(i, key1);
    update_key(i - 1, key2);
    update_key(i - 2, key3);

    //get some space for these values
    size_t* v1 = malloc(sizeof(size_t));
    size_t* v2 = malloc(sizeof(size_t));
    void* tmp1 = v1;
    void* tmp2 = v2;

    //get first value
    if((v1 = mcache_get(key2)) == NULL) {
      //printf("cache miss on %d\n", i - 1);
      v1 = tmp1;
      *v1 = fibonacci(i - 1);
    }

    //get second value
    if((v2 = mcache_get(key3)) == NULL) {
      //printf("cache miss on %d\n", i - 2);
      v2 = tmp2;
      *v2 = fibonacci(i - 2);
    }

    //store tmp values
    size_t final = *v1 + *v2;

    //update cache to have new value
    mcache_add(key1, &final, sizeof(final));

    //just return if final
    if(i == n) {
      return final;
    }
  }

  //not reachable, one would hope
  return 1234567;
}

size_t fibonacci2(int n) {
  if(n == 0 || n == 1) {
    return n;
  } else {
    return fibonacci2(n - 1) + fibonacci2(n - 2);
  }
}

int main(int argc, char** argv) {
  if(argc != 2) {
    fprintf(stderr, "usage: %s <n>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  int n = atoi(argv[1]);
  if(n <= 0) { return 2; }
  fibonacci2(n);
  return 0;

  mcache_init("localhost");

  printf("Computing the %dth number in the fibonacci sequence...\n", n);
  printf("The %dth number is: %zu\n", n, fibonacci(n));
  printf("Cache misses: %zu\n", mcache_get_cache_misses());
  /*
  printf("Now, not using mcache...\n");
  printf("%llu\n", fibonacci2(n));
  */
  mcache_exit();
}
