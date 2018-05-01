#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "entry_list.h"
#include <pthread.h>

//Fixed size hash map using chaining for collisions

#define INITIAL_CAPACITY 1000

// This makes the header file work for both C and C++
#ifdef __cplusplus
extern "C" {
#endif

  typedef struct _bucket {
    entry_list_t* elist;
    pthread_mutex_t m;
  } bucket_t;

  typedef struct _hashmap {
    int capacity;
    int size_used;
    bucket_t** table;
    pthread_mutex_t map_lock;
  } hashmap_t;

  // Initialize a hashmap
  void hashmap_init(hashmap_t* map);

  // Destoy an bucket
  void hashmap_bucket_destroy(bucket_t* element);

  // Destroy a hashmap
  void hashmap_destroy(hashmap_t* map);

  // Check if hashmap contains key
  bool hashmap_contains_key(hashmap_t* map, char* key);

  // Get a value by key
  void* hashmap_get(hashmap_t* map, char* key);

  // Get the key set for given map
  char** hashmap_key_set(hashmap_t* map);

  // Determine whether or not map is empty
  bool hashmap_is_empty(hashmap_t* map);

  // Determine the size of the map
  int hashmap_size(hashmap_t* map);

  // Put element into map -- if key already exists, update value
  void hashmap_put(hashmap_t* map, char* key, byte_sequence_t* value);

  // Remove an element from map
  void hashmap_remove(hashmap_t* map, char* key);

  // This makes the header file work for both C and C++
#ifdef __cplusplus
}
#endif

#endif
