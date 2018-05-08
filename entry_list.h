#ifndef ENTRY_LIST_H
#define ENTRY_LIST_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>

#include "mcache_types.h"

// This makes the header file work for both C and C++
#ifdef __cplusplus
extern "C" {
#endif

  typedef struct entry_node {
    char* key;
    byte_sequence_t* data;
    struct entry_node* next;
  } entry_node_t;

  /* Note: entries are ordered alphabetically by key */
  typedef struct entry_list {
    entry_node_t* first;
    size_t length;
  } entry_list_t;

  // Initialize a elist
  void elist_init(entry_list_t* elist);

  // Destroy a elist
  void elist_destroy(entry_list_t* elist);

  // Push an element onto a elist
  void elist_push(entry_list_t* elist, char* key, byte_sequence_t* element);

  // Push an element onto an elist if it does not already exist
  void elist_offer(entry_list_t* elist, char* key, byte_sequence_t* element);

  // Push a unique element onto a elist
  void elist_push_unique(entry_list_t* elist, char* key, byte_sequence_t* element);

  // Check if a elist is empty
  bool elist_empty(entry_list_t* elist);

  // Remove element from elist
  void elist_remove(entry_list_t* elist, char* key);

  // Get a byte_sequence_t from the list, given key
  byte_sequence_t* elist_get(entry_list_t* elist, char* key);

  // This makes the header file work for both C and C++
#ifdef __cplusplus
}
#endif

#endif
