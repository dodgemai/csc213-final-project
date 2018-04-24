#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "entry_list.h"

//Fixed size hash map using chaining
//Need to implement a linked list,
//and have elements contain that linked-list

#define INITIAL_CAPACITY 100

// This makes the header file work for both C and C++
#ifdef __cplusplus
extern "C" {
#endif  

  typedef struct hashmap {
    int capacity;
    int size_used;
    entry_list_t** table;
  } hashmap_t;
  
  // Initialize a hashmap
  void hashmap_init(hashmap_t* map);

  // Destoy an element
  void hashmap_element_destroy(entry_list_t* element);
  
  // Destroy a hashmap
  void hashmap_destroy(hashmap_t* map);

  // Check if hashmap contains key
  bool hashmap_contains_key(hashmap_t* map, const char* key);

  // Get a value by key
  void* hashmap_get(hashmap_t* map, const char* key);

  // Get the key set for given map
  char** hashmap_key_set(hashmap_t* map);

  // Determine whether or not map is empty
  bool hashmap_is_empty(hashmap_t* map);

  // Determine the size of the map
  int hashmap_size(hashmap_t* map);
  
  // This makes the header file work for both C and C++
#ifdef __cplusplus
}
#endif

#endif
