#include "mcache.h"
#include <string.h>
#include <stdio.h>
typedef struct _big_struct {
  int i1;
  int i2;
  char c;
  char* str;
} big_struct_t;

int main(void) {
  mcache_init("localhost");
  byte_sequence_t* formatted_test = (byte_sequence_t*) malloc(sizeof(byte_sequence_t));
  char* test = strdup("value1");
  formatted_test->data = test;
  formatted_test->length = strlen(test) + 1;

  printf("Set as: %s\n", formatted_test->data);
  mcache_set("key1", formatted_test);

  char* v1 = mcache_get("key1")->data;

  printf("Retrieved as: %s\n", v1);
  mcache_exit();
}
