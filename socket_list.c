#include "socket_list.h"
#include <stdlib.h>

// Initialize a slist
void slist_init(socket_list_t* slist) {
  slist->first = NULL;
  slist->length = 0;
}

// Destroy a slist
void slist_destroy(socket_list_t* slist) {
  sock_node_t* cur = slist->first;
  while(cur != NULL) {
    sock_node_t* tmp = cur->next;
    free(cur);
    cur = tmp;
  }
}

// Push an element onto a slist
void slist_push(socket_list_t* slist, int element) {
  sock_node_t* to_add = (sock_node_t*) malloc(sizeof(sock_node_t));
  to_add->data = element;
  to_add->next = slist->first;
  slist->first = to_add;
  slist->length++;
}

// Check if a slist is empty
bool slist_empty(socket_list_t* slist) {
  bool ret = slist->first == NULL;
  return ret;
}

// Remove an element form slist
void slist_remove(socket_list_t* slist, int socket) {
  if(slist_empty(slist)) {
    return;
  }

  sock_node_t* cur = slist->first;
  sock_node_t* prev = NULL;
  while(cur != NULL) {
    //if found, remove
    if(cur->data == socket) {
      slist->length--;
      if(prev == NULL) {
        slist->first = NULL; //if no prev, list is empty
        free(cur);
        return;
      } else {
        prev->next = cur->next; //set prev to skip over cur
        free(cur);
        return;
      }
    }
    prev = cur;
    cur = cur->next;
  }
}
