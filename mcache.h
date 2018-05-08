#ifndef MCACHE_H
#define MCACHE_H

#include <stdbool.h>
#include "entry_list.h"

//NOTE TODO might want to separate byte_sequence_t into a more relevant header file!

// Initialize the mcache server
void mcache_init(char* server_address);

// Adds data into the mcache -- if the key already exists, update value
void mcache_set(char* key, void* data_ptr, size_t data_len);

// Adds data into the mcache if the key does not already exist
void mcache_add(char* key, void* data_ptr, size_t data_len);

// Gets a value from the mcache by key
// returns NULL on failure
void* mcache_get(char* key);

// Gets an array of values from mcache by an array of keys
byte_sequence_t** mcache_gets(char** keys, size_t num_keys);

// Deletes data stored in the mcache by key
void mcache_delete(char* key);

// Close the mcache server
void mcache_exit(void);
#endif
