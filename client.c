#include "mcache.h"
#include <string.h>
#include <stdio.h>

void print_big_struct(big_struct_t* s) {
  printf("i1 = %d\n i2 = %d\n c = %c\n str = %s\n\n", s->i1, s->i2, s->c, s->str);
}
int main(void) {
  mcache_init("localhost");
  //byte_sequence_t* formatted_test = (byte_sequence_t*) malloc(sizeof(byte_sequence_t));
  char* test = strdup("value1");

  printf("Testing with char*...\n");
  printf("Set as: %s\n", test);
  mcache_add("key1", test, strlen(test) + 1); // + 1 for null terminator

  char* v1 = mcache_get("key1");

  printf("Retrieved as: %s\n\n", v1);

  //NOTE user is responsible for freeing returned data!!!
  free(v1);

  printf("Testing with struct...\n");
  printf("Added as: ");

  //set up struct with some values
  big_struct_t test2 = {
    .i1 = 1,
    .i2 = 69,
    .c = 'g'
  };
  test2.str[0] = 'h';
  test2.str[1] = 'i';
  test2.str[2] = '\0';

  print_big_struct(&test2);
  mcache_add("key2", &test2, sizeof(test2));

  big_struct_t* v2 = mcache_get("key2");

  printf("Retrieved as: ");
  print_big_struct(v2);
  free(v2);

  mcache_exit();
}
