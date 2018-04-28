#ifndef MCACHE_H
#define MCACHE_H

#include <stdbool.h>
#include <entry_list.h>

//NOTE TODO might want to separate byte_sequence_t into a more relevant header file!

bool mcache_set(char* key, byte_sequence_t* data);

bool mcache_add(char* key, byte_sequence_t* data);

byte_sequence_t* mcache_get(char* key);

byte_sequence_t** mcache_gets(char** keys, size_t num_keys);

bool mcache_delete(char* key);

#endif
