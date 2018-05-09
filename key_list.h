#ifndef KEY_LIST_H
#define KEY_LIST_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/*
 * Doubly linked queue
 */

// This makes the header file work for both C and C++
#ifdef __cplusplus
extern "C" {
#endif

  typedef struct key_data {
    char* key;
    size_t data_size;
  } key_data_t;

  typedef struct key_node {
    key_data_t* data;
    struct key_node* next;
    struct key_node* prev;
  } key_node_t;

  typedef struct key_list {
    key_node_t* first;
    key_node_t* last;
    size_t length;
  } key_list_t;

  // Initialize a klist
  void klist_init(key_list_t* klist);

  // Destroy a klist
  void klist_destroy(key_list_t* klist);

  // Push an element onto the front of a klist
  void klist_add(key_list_t* klist, char* element, size_t obj_size);

  // Check if a klist is empty
  bool klist_empty(key_list_t* klist);

  //pop the last element off of the klist
  key_data_t* klist_poll(key_list_t* klist);

  //remove specific element from klist
  key_data_t* klist_remove(key_list_t* klist, char* key);

  // This makes the header file work for both C and C++
#ifdef __cplusplus
}
#endif

#endif
