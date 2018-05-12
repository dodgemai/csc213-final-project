#define NDEBUG = false

#include "mcache.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <stdbool.h>

#define NUM_THREADS 10
#define NUM_VALS 1000
#define IN_CLIENT true

typedef struct bool_args {
  bool b;
} bool_args_t;

// Basic mcache test functionality with int values
void* basic_mcache_test_int(void* args) {
  if(!IN_CLIENT) {
    mcache_init("localhost");
  }

  bool parallel = ((bool_args_t*)args)->b;
  //free(args);

  for(int i = 0; i <= NUM_VALS; i++) {
    char key[25];
    sprintf(key, "key%d", i);
    mcache_add(key, &(i), sizeof(i));
    int* val = (int*)mcache_get(key);
    if(val == NULL && parallel) { continue; }
    assert(*(val) == i);
    free(val);
  }

  int successful_gets = 0;
  for(int i = 0; i <= NUM_VALS; i++) {
    char key[25];
    sprintf(key, "key%d", i);
    int* val;
    if((val = (int*)mcache_get(key)) != NULL) {
      assert(*val == i);
      free(val);
      successful_gets++;
    }
  }

  if(!parallel) {
    assert((successful_gets == MCACHE_MAX_ALLOCATION / 4) || successful_gets == NUM_VALS + 1);
  }

  if(!IN_CLIENT) {
    mcache_exit();
  }
  return NULL;
}

// Basic mcache test functionality with struct values
void* basic_mcache_test_struct(void* args) {
  if(!IN_CLIENT) {
    mcache_init("localhost");
  }
  bool parallel = ((bool_args_t*)args)->b;
  //free(args);

  for(int i = 0; i <= NUM_VALS; i++) {
    char key[25];
    sprintf(key, "key%d", i);

    //set up struct with some values
    big_struct_t valin = {
      .i1 = i,
      .i2 = NUM_VALS - i,
      .c = key[4]
    };
    strcpy(valin.str, key);

    mcache_add(key, &valin, sizeof(valin));

    big_struct_t* valout = (big_struct_t*)mcache_get(key);
    if(valout == NULL && parallel) { continue; }
    //assert equality of each component
    assert(valin.i1 == valout->i1);
    assert(valin.i2 == valout->i2);
    assert(valin.c == valout->c);
    assert(!strcmp(valin.str, valout->str));

    free(valout);
  }

  int successful_gets = 0;
  for(int i = 0; i <= NUM_VALS; i++) {
    char key[25];
    sprintf(key, "key%d", i);
    big_struct_t* val;
    if((val = (big_struct_t*)mcache_get(key)) != NULL) {
      assert(val->i1 == i);
      assert(val->i2 == NUM_VALS - i);
      assert(val->c == key[4]);
      assert(!strcmp(val->str, key));
      free(val);
      successful_gets++;
    }
  }

  if(!parallel) {
    assert((successful_gets == MCACHE_MAX_ALLOCATION / sizeof(big_struct_t))
    || successful_gets == NUM_VALS + 1);
  }

  if(!IN_CLIENT) {
    mcache_exit();
  }
  return NULL;
}

// Basic mcache test functionality with char* value (basically testing for arrays too)
void* basic_mcache_test_arr(void* args) {
  if(!IN_CLIENT) {
    mcache_init("localhost");
  }
  bool parallel = ((bool_args_t*)args)->b;

  for(int i = 0; i <= NUM_VALS; i++) {
    char key[25];
    sprintf(key, "key%d", i);
    //char* valin = strdup(key);
    mcache_add(key, key, strlen(key) + 1);
    char* valout = (char*)mcache_get(key);
    if(valout == NULL && parallel) { continue; }
    //check c-string equality
    assert(!strcmp(valout, key));

    //free vals
    //free(valin);
    free(valout);
  }

  int successful_gets = 0;
  for(int i = 0; i <= NUM_VALS; i++) {
    char key[25];
    sprintf(key, "key%d", i);
    char* val;
    if((val = (char*)mcache_get(key)) != NULL) {
      //check c-string equality
      assert(!strcmp(val, key));
      free(val);
      successful_gets++;
    }
  }

  //assert((successful_gets == MCACHE_MAX_ALLOCATION / sizeof(char)) || successful_gets == NUM_VALS + 1);
  if(!IN_CLIENT) {
    mcache_exit();
  }
  return NULL;
}

int main(void) {
  if(IN_CLIENT) {
    mcache_init("localhost");
  }

  //basic test:
  printf("Testing sequential correctness...\n");
  bool_args_t b = { .b = false };
  basic_mcache_test_int(&b);
  basic_mcache_test_struct(&b);
  basic_mcache_test_arr(&b);

  /* NOTE: can't test parallel correctness like this. all will be using the
  same socket and things will get super messed up. Oops. Could test it by
  forking this program however many times. I don't have time to do that though!!

  printf("Testing parallel correctness...\n");

  //get space for threads
  pthread_t int_test[NUM_THREADS];
  pthread_t struct_test[NUM_THREADS];
  pthread_t arr_test[NUM_THREADS];

  bool_args_t b = { .b = true };

  for(int i = 0; i < NUM_THREADS; i++) {
    if(pthread_create(&(int_test[i]), NULL, basic_mcache_test_int, &b)) {
      perror("Failed to create child thread");
      exit(2);
    }
    if(pthread_create(&struct_test[i], NULL, basic_mcache_test_struct, &b)) {
      perror("Failed to create child thread");
      exit(2);
    }
    if(pthread_create(&arr_test[i], NULL, basic_mcache_test_arr, &b)) {
      perror("Failed to create child thread");
      exit(2);
    }
  }

  for(int i = 0; i < NUM_THREADS; i++) {
    if(pthread_join(int_test[i], NULL)) {
      perror("Failed to create child thread");
      exit(2);
    }
    if(pthread_join(struct_test[i], NULL)) {
      perror("Failed to create child thread");
      exit(2);
    }
    if(pthread_join(arr_test[i], NULL)) {
      perror("Failed to create child thread");
      exit(2);
    }
  }
  if(IN_CLIENT) {
    mcache_exit();
  }
  */
}
