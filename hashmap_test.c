#include "hashmap.h"
#include <stdio.h>
#include <string.h>

int main(void) {
  //declare and init map
  hashmap_t map;
  hashmap_init(&map);

  char* key1 = strdup("key1");
  char* value1 = strdup("value1");

  char* key2 = strdup("key2");
  char* value2 = strdup("value2");

  byte_sequence_t* value1_byteseq;
  value1_byteseq->data = value1;
  value1_byteseq->length = 69;

  byte_sequence_t* value2_byteseq;
  value2_byteseq->data = value2;
  value2_byteseq->length = 692;

  hashmap_put(&map, key1, value1_byteseq);
  hashmap_put(&map, key2, value2_byteseq);

  byte_sequence_t* value1_ret = (byte_sequence_t*) hashmap_get(&map, key1);
  byte_sequence_t* value2_ret = (byte_sequence_t*) hashmap_get(&map, key2);

  printf("Value 1: %s\n", (char*)value1_ret->data);

  return 0;
}
