#include "key_list.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


void klist_init(key_list_t* klist) {
  klist->first = NULL;
  klist->length = 0;
}

void klist_destroy(key_list_t* klist) {
  key_node_t* cur = klist->first;
  while(cur != NULL) {
    key_node_t* tmp = cur->next;
    free(cur);
    cur = tmp;
  }
}

void knode_destroy(key_node_t* knode) {
  if(knode != NULL) {
    //free(knode->data->key);
    //free(knode->data);
    free(knode);
  }
}

key_node_t* knode_make(char* element, size_t obj_size, key_node_t* prev, key_node_t* next) {

  //allocate space for node
  key_node_t* ret = (key_node_t*) malloc(sizeof(key_node_t));

  //allocate space for key_data
  ret->data = malloc(sizeof(key_data_t));

  //fill in data for node
  ret->data->key = strdup(element);
  ret->data->data_size = obj_size;

  //fill in prev and next
  ret->prev = prev;
  ret->next = next;

  return ret;
}

void klist_add(key_list_t* klist, char* element, size_t obj_size) {
  /* Iterate through and remove duplicate keys */
  key_node_t* cur = klist->first;
  if(cur != NULL) {
    //is the first entry a duplicate?
    if(strcmp(cur->data->key, element) == 0) {
      klist->first = cur->next;
      knode_destroy(cur);
      cur = NULL;
      klist->length--;
    }

    //check the rest for duplicates
    while(cur != NULL) {
      if(strcmp(cur->data->key, element) == 0) {
        cur->prev->next = cur->next;
        if(cur->next != NULL) {
          cur->next->prev = cur->prev;
        } else {
          klist->last = cur->prev;
        }
        knode_destroy(cur);
        klist->length--;
        break;
      }
      cur = cur->next;
    }
  }
  /* Add key to the front of the queue */
  key_node_t* to_add = knode_make(element, obj_size, NULL, klist->first);

  if(to_add->next != NULL) {
    to_add->next->prev = to_add;
  } else {
    klist->last = to_add;
  }
  klist->first = to_add;
  klist->length++;
}


bool klist_empty(key_list_t* klist) {
  if(klist == NULL) { return true; }
  return klist->first == NULL;
}

key_data_t* klist_poll(key_list_t* klist) {
  //no list
  if(klist == NULL) { return NULL; }

  //empty list
  if(klist->last == NULL) { return NULL; }

  key_node_t* new_last = klist->last->prev;
  key_data_t* ret = klist->last->data;

  //singleton list
  if(new_last == NULL) {
    knode_destroy(klist->last);
    klist->last = NULL;
    klist->first = NULL;
    klist->length--;
    return ret;
  }

  //'normal' list (ie length > 1)
  new_last->next = NULL;

  knode_destroy(klist->last);

  klist->last = new_last;
  klist->length--;

  return ret;
}

key_data_t* klist_remove(key_list_t* klist, char* key) {
  key_node_t* cur = klist->first;
  key_data_t* ret = NULL;
  if(cur != NULL) {
    //is the first entry a match?
    if(strcmp(cur->data->key, key) == 0) {
      klist->first = cur->next;
      ret = cur->data;
      free(cur);
      cur = NULL;
      klist->length--;
    }

    //check the rest for match
    while(cur != NULL) {
      if(strcmp(cur->data->key, key) == 0) {
        cur->prev->next = cur->next;
        if(cur->next != NULL) {
          cur->next->prev = cur->prev;
        } else {
          klist->last = cur->prev;
        }
        ret = cur->data;
        free(cur);
        klist->length--;
        break;
      }
      cur = cur->next;
    }
  }

  return ret;
}