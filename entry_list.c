#include "entry_list.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Initialize a elist
void elist_init(entry_list_t* elist) {
  if(elist == NULL) { return; }
  elist->first = NULL;
  elist->length = 0;
}

// Destroy an element of the elist
void entry_destroy(entry_node_t* node) {
  free(node->key);
  free(node->data);
  free(node);
}

// Destroy a elist
void elist_destroy(entry_list_t* elist) {
  if(elist == NULL) { return; }

  entry_node_t* cur = elist->first;
  while(cur != NULL) {
    entry_node_t* tmp = cur->next;
    entry_destroy(cur);
    cur = tmp;
  }
}

// Allocate memory and create a node for entry
entry_node_t* make_entry_node(char* key, byte_sequence_t* element, entry_node_t* next) {
  entry_node_t* node = (entry_node_t*) malloc(sizeof(entry_node_t));
  if(node == NULL) {
    fprintf(stderr, "Failed to allocate memory for entry node.\n");
    exit(EXIT_FAILURE);
  }
  node->data = element;
  node->next = next;
  node->key = strdup(key);
  return node;
}

// Push an element onto an elist
void elist_push(entry_list_t* elist, char* key, byte_sequence_t* element) {
  if(elist == NULL) { return; }
  entry_node_t* to_add = make_entry_node(key, element, elist->first);
  elist->first = to_add;
  elist->length++;
}

// Push an element onto an elist if it does not already exist
void elist_offer(entry_list_t* elist, char* key, byte_sequence_t* element) {
  if(elist == NULL) { return; }
  if(elist_empty(elist)) {
    elist_push(elist, key, element);
  }

  entry_node_t* cur = elist->first;
  for(int i = 0; i < elist->length; i++) {
    int comp_val = strcmp(cur->key, key);
    if(comp_val == 0) {
      return;
    } else if(comp_val < 0) {
      entry_node_t* to_add = make_entry_node(key, element, cur->next);
      cur->next = to_add;
      elist->length++;
      return;
    }
    if(cur->next != NULL) {
      cur = cur->next;
    } else {
      entry_node_t* to_add = make_entry_node(key, element, NULL);
      cur->next = to_add;
      elist->length++;
      return;
    }
  }
}

// Push a unique element onto a elist, overwrite if exists
void elist_push_unique(entry_list_t* elist, char* key, byte_sequence_t* element) {
  if(elist == NULL) { return; }
  if(elist_empty(elist)) {
    elist_push(elist, key, element);
  }

  entry_node_t* cur = elist->first;
  for(int i = 0; i < elist->length; i++) {
    int comp_val = strcmp(cur->key, key);
    if(comp_val == 0) {
      cur->data = element;
      return;
    } else if(comp_val < 0) {
      entry_node_t* to_add = make_entry_node(key, element, cur->next);
      cur->next = to_add;
      elist->length++;
      return;
    }
    if(cur->next != NULL) {
      cur = cur->next;
    } else {
      entry_node_t* to_add = make_entry_node(key, element, NULL);
      cur->next = to_add;
      elist->length++;
      return;
    }
  }
}

// Check if a elist is empty
bool elist_empty(entry_list_t* elist) {
  if(elist == NULL) { return true; }
  bool ret = elist->first == NULL;
  return ret;
}

// Remove element from elist
void elist_remove(entry_list_t* elist, char* key) {
  if(elist == NULL) { return; }
  if(elist_empty(elist)) {
    return;
  }

  entry_node_t* cur = elist->first;
  entry_node_t* prev = NULL;
  while(cur != NULL) {
    //if found, remove
    if(strcmp(cur->key, key) == 0) {
      elist->length--;
      if(prev == NULL) {
        elist->first = NULL; //if no prev, list is empty
        entry_destroy(cur);
        return;
      } else {
        prev->next = cur->next; //set prev to skip over cur
        entry_destroy(cur);
        return;
      }
    }
    prev = cur;
    cur = cur->next;
  }
}

// Get a byte_sequence_t from the list, given key
byte_sequence_t* elist_get(entry_list_t* elist, char* key) {
  if(elist == NULL) { return NULL; }
  if(elist_empty(elist)) {
    return NULL;
  }
  entry_node_t* cur = elist->first;

  //loop through all elements of list
  while(cur != NULL) {
    //if we find it, return the corresponding byte sequence
    if(strcmp(cur->key, key) == 0) {
      return cur->data;
    }
    cur = cur->next;
  }

  //didn't find it
  return NULL;
}
