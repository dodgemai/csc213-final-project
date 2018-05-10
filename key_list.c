#include "key_list.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//NOTE TODO make thread-safety a little less shittily/lazily implemented

//init a key list
void klist_init(key_list_t* klist) {
  pthread_mutex_init(&klist->m, NULL);
  klist->first = NULL;
  klist->last = NULL;
  klist->length = 0;
}

//destroy a key list
void klist_destroy(key_list_t* klist) {
  pthread_mutex_lock(&klist->m);
  key_node_t* cur = klist->first;
  while(cur != NULL) {
    key_node_t* tmp = cur->next;
    free(cur);
    cur = tmp;
  }
  pthread_mutex_unlock(&klist->m);
  pthread_mutex_destroy(&klist->m);
  free(klist);
}

//destroy a key node
void knode_destroy(key_node_t* knode) {
  if(knode != NULL) {
    free(knode->data->key);
    free(knode->data);
    free(knode);
  }
}

//make a key node
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

// Push an element onto the front of a klist
void klist_add(key_list_t* klist, char* element, size_t obj_size) {
  if(klist == NULL) { return; }

  pthread_mutex_lock(&klist->m);
  /* Iterate through and remove duplicate keys */
  key_node_t* cur = klist->first;
  if(cur != NULL) {
    //is the first entry a duplicate?
    if(strcmp(cur->data->key, element) == 0) {
      klist->first = cur->next;
      if(klist->first == NULL) { klist->last = NULL; }
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

  pthread_mutex_unlock(&klist->m);
}

// Check if a klist is empty
bool klist_empty(key_list_t* klist) {
  bool ret;
  if(klist == NULL) {
    ret = true;
  } else {
    pthread_mutex_lock(&klist->m);
    ret = klist->first == NULL;
    pthread_mutex_unlock(&klist->m);
  }
  return klist->first == NULL;
}

//pop the last element off of the klist -- User is responsible for freeing data
key_data_t* klist_poll(key_list_t* klist) {
  //no list
  if(klist == NULL) { return NULL; }
  pthread_mutex_lock(&klist->m);
  //empty list
  if(klist->last == NULL) { return NULL; }

  key_node_t* new_last = klist->last->prev;
  key_data_t* ret = klist->last->data;

  //singleton list
  if(new_last == NULL) {
    free(klist->last); //note user must free the rest of the data
    klist->last = NULL;
    klist->first = NULL;
    klist->length--;
    return ret;
  }

  //'normal' list (ie length > 1)
  new_last->next = NULL;

  free(klist->last); //note user must free the rest of the data

  klist->last = new_last;
  klist->length--;
  pthread_mutex_unlock(&klist->m);
  return ret;
}

//remove specific element from klist
key_data_t* klist_remove(key_list_t* klist, char* key) {
  if(klist == NULL) { return NULL; }
  pthread_mutex_lock(&klist->m);
  key_node_t* cur = klist->first;
  key_data_t* ret = NULL;
  if(cur != NULL) {
    //is the first entry a match?
    if(strcmp(cur->data->key, key) == 0) {
      klist->first = cur->next;
      if(klist->first == NULL) { klist->last = NULL; }
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
  pthread_mutex_unlock(&klist->m);
  return ret;
}
