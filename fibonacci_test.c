#include "mcache.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

//update key with new n value -- note: key should already be null terminated
void update_key(int n, char* key) {
  memcpy(key, &n, sizeof(n));
}

unsigned long long fibonacci(int n) {
  //key is basically just gonna be the bytes of int and a null terminator
  char* key1 = malloc(sizeof(int) + 1);
  char* key2 = malloc(sizeof(int) + 1);
  char* key3 = malloc(sizeof(int) + 1);
  key1[sizeof(int)] = '\0';
  key2[sizeof(int)] = '\0';
  key3[sizeof(int)] = '\0';

  //check to see if n value has already been cached...
  update_key(n, key1);
  unsigned long long* v = mcache_get(key1);

  //if cached, free val and return it
  if(v != NULL) {
    unsigned long long tmp = *v;
    free(v);
    free(key1);
    free(key2);
    free(key3);
    return tmp;
  }

  int f0 = 0, f1 = 1;
  update_key(f0, key2);
  mcache_add(key2, &(unsigned long long) { 0 }, sizeof(unsigned long long));

  update_key(f1, key3);
  mcache_add(key3, &(unsigned long long) { 1 }, sizeof(unsigned long long));

  for(int i = 2; i <= n; i++) {
    //update keys
    update_key(i, key1);
    update_key(i - 1, key2);
    update_key(i - 2, key3);

    //get some space for these values
    unsigned long long* v1 = malloc(sizeof(unsigned long long));
    unsigned long long* v2 = malloc(sizeof(unsigned long long));
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
    unsigned long long final = *v1 + *v2;

    //update cache to have new value
    mcache_add(key1, &final, sizeof(final));

    //just return if final
    if(i == n) {
      free(key1);
      free(key2);
      free(key3);
      return final;
    }
  }

  //not reachable, one would hope
  return 1234567;
}

unsigned long long fibonacci2(int n) {
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

  mcache_init("localhost");

  printf("Computing the %dth number in the fibonacci sequence...\n", n);
  printf("The %dth number is: %llu\n", n, fibonacci(n));
  printf("Cache misses: %zu\n", mcache_get_cache_misses());
  /*
  printf("Now, not using mcache...\n");
  printf("%llu\n", fibonacci2(n));
  */
  mcache_exit();
}
