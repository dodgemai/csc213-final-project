#include <gtest/gtest.h>

#include "mcache.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>

#include <thread>
#include <vector>

#define NUM_THREADS 100
#define NUM_VALS 1000
// Basic mcache test functionality with int values
TEST(MCacheTest, BasicMCacheTestInt) {
  mcache_init("localhost");

  for(int i = 0; i <= NUM_VALS; i++) {
    char key[25];
    sprintf(key, "key%d", i);
    mcache_add(key, &(i), sizeof(i));
    ASSERT_EQ(*((int*)mcache_get(key)), i);
  }

  int successful_gets = 0;
  for(int i = 0; i <= NUM_VALS; i++) {
    char key[25];
    sprintf(key, "key%d", i);
    int* val;
    if((val = (int*)mcache_get(key)) != NULL) {
      ASSERT_EQ(*val, i);
      free(val);
      successful_gets++;
    }
  }

  ASSERT_TRUE((successful_gets == MCACHE_MAX_ALLOCATION / 4) || successful_gets == NUM_VALS + 1);

  mcache_exit();
}

// Basic mcache test functionality with struct values
TEST(MCacheTest, BasicMCacheTestStruct) {
  mcache_init("localhost");

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
    ASSERT_EQ(valin.i1, valout->i1);
    ASSERT_EQ(valin.i2, valout->i2);
    ASSERT_EQ(valin.c, valout->c);
    ASSERT_STREQ(valin.str, valout->str);

    free(valout);
  }

  int successful_gets = 0;
  for(int i = 0; i <= NUM_VALS; i++) {
    char key[25];
    sprintf(key, "key%d", i);
    big_struct_t* val;
    if((val = (big_struct_t*)mcache_get(key)) != NULL) {
      ASSERT_EQ(val->i1, i);
      ASSERT_EQ(val->i2, NUM_VALS - i);
      ASSERT_EQ(val->c, key[4]);
      ASSERT_STREQ(val->str, key);
      free(val);
      successful_gets++;
    }
  }

  ASSERT_TRUE((successful_gets == MCACHE_MAX_ALLOCATION / 4) || successful_gets == NUM_VALS + 1);

  mcache_exit();
}

// Basic mcache test functionality with char* value (basically testing for arrays too)
TEST(MCacheTest, BasicMCacheTestCString) {
  mcache_init("localhost");

  for(int i = 0; i <= NUM_VALS; i++) {
    char key[25];
    sprintf(key, "key%d", i);
    char* valin = strdup(key);
    mcache_add(key, &(valin), strlen(valin));
    char* valout = (char*)mcache_get(key);
    ASSERT_STREQ(valout, valin);
    free(valin);
    free(valout);
  }

  int successful_gets = 0;
  for(int i = 0; i <= NUM_VALS; i++) {
    char key[25];
    sprintf(key, "key%d", i);
    char* val;
    if((val = (char*)mcache_get(key)) != NULL) {
      ASSERT_STREQ(val, key);
      free(val);
      successful_gets++;
    }
  }

  ASSERT_TRUE((successful_gets == MCACHE_MAX_ALLOCATION / 4) || successful_gets == NUM_VALS + 1);

  mcache_exit();
}
