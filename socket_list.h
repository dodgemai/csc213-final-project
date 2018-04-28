#ifndef SOCKET_LIST_H
#define SOCKET_LIST_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// This makes the header file work for both C and C++
#ifdef __cplusplus
extern "C" {
#endif

  typedef struct sock_node {
    int data;
    struct sock_node* next;
  } sock_node_t;

  typedef struct socket_list {
    sock_node_t* first;
    size_t length;
  } socket_list_t;

  // Initialize a slist
  void slist_init(socket_list_t* slist);

  // Destroy a slist
  void slist_destroy(socket_list_t* slist);

  // Push an element onto a slist
  void slist_push(socket_list_t* slist, int element);

  // Check if a slist is empty
  bool slist_empty(socket_list_t* slist);

  //remove element from slist
  void slist_remove(socket_list_t* slist, int socket);

  // This makes the header file work for both C and C++
#ifdef __cplusplus
}
#endif

#endif
