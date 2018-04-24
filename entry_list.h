#ifndef ENTRY_LIST_H
#define ENTRY_LIST_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// This makes the header file work for both C and C++
#ifdef __cplusplus
extern "C" {
#endif
  //TODO: make this actually work for entries -- store entries rather than ints!

  typedef struct hashmap_element {
    char* key;
    bool in_use;
    void* data; 
  } hashmap_element_t;
  
  typedef struct entry_node {
    hashmap_element_t* data;
    struct entry_node* next; 
  } entry_node_t;

  typedef struct entry_list {
    entry_node_t* first;
    size_t length;
  } entry_list_t;
  
  // Initialize a elist
  void elist_init(entry_list_t* slist);

  // Destroy a elist
  void elist_destroy(entry_list_t* slist);

  // Push an element onto a elist
  void elist_push(entry_list_t* slist, hashmap_element_t* element);

  // Check if a elist is empty
  bool elist_empty(entry_list_t* slist);

  //remove element from elist
  void elist_remove(entry_list_t* slist, char* key);

  // This makes the header file work for both C and C++
#ifdef __cplusplus
}
#endif

#endif
