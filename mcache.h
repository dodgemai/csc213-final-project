#ifndef MCACHE_H
#define MCACHE_H

#include <stdbool.h>
#include <entry_list.h>

//NOTE TODO might want to separate byte_sequence_t into a more relevant header file!

bool mcache_set(hashmap_t* map, char* key, byte_sequence_t* data);

bool mcache_add(hashmap_t* map, char* key, byte_sequence_t* data);

byte_sequence_t* mcache_get(hashmap_t* map, char* key);

byte_sequence_t** mcache_gets(hashmap_t* map, char** keys, size_t num_keys);

bool mcache_delete(hashmap_t* map, char* key);

#endif
