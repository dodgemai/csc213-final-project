#include "mcache.h"
#include <string.h>
#include <stdio.h>

int main(void) {
  mcache_init("localhost");
  byte_sequence_t* formatted_test = (byte_sequence_t*) malloc(sizeof(byte_sequence_t));
  char* test = strdup("balls");
  formatted_test->data = test;
  formatted_test->length = strlen(test) + 1;

  printf("Set as: %s\n", formatted_test->data);
  mcache_set("key1", formatted_test);
  mcache_set("key1", formatted_test);
  
  char* gotten_bennett = mcache_get("key1")->data;

  printf("Retrieved as: %s\n", gotten_bennett);
  mcache_exit();
}
