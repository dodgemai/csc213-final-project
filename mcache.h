#ifndef MCACHE_H
#define MCACHE_H

#include <stdbool.h>
#include "entry_list.h"

//NOTE TODO might want to separate byte_sequence_t into a more relevant header file!

void mcache_init(char* server_address);

void mcache_set(char* key, void* data_ptr, size_t data_len);

void mcache_add(char* key, void* data_ptr, size_t data_len);

void* mcache_get(char* key);

byte_sequence_t** mcache_gets(char** keys, size_t num_keys);

void mcache_delete(char* key);

void mcache_exit(void);
#endif
