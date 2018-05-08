#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>

#include "hashmap.h"
#include "socket_list.h"
#include "key_list.h"
#include "server.h"
#include "mcache_types.h"

#define _DEBUG false
/* NOTE: maybe don't actually need g_child_socks -- we will see.
* we can keep for now, but I really don't foresee a need for it to exist
*/

/**
* Thread function to read input from child thread
*/
void* child_thread_fn(void* arg);

/**
* remove any newlines from the message string
*/
void trim_message(char* message);

/**
* get mcache data length from a character string
* NOTE: will almost certainly segfault if using this outside
* of context of mcache data being sent over tcp connection
*/
size_t mcache_data_len(char* message);
void parse_query(char* query, int socket);
void parse_set(char* args);
void parse_add(char* args);
void parse_get(char* args, int socket);
void parse_gets(char* args, int socket);
void parse_delete(char* args);
void touch_key(char* key, size_t obj_size);
//handle dumb SIGPIPE signal
void handler(int s) { }

//global values
socket_list_t g_child_socks;
hashmap_t g_hmap;
bool g_active;
key_list_t g_keys;
size_t g_memory_allocated;

typedef struct socket_arg {
  int socket;
} socket_arg_t;

int main(void) {

  slist_init(&g_child_socks); //prep g_child_socks
  hashmap_init(&g_hmap); //prep g_hmap
  klist_init(&g_keys);  //prep g_keys
  g_active = true; //set server to active
  g_memory_allocated = 0; //init mem_allocated

  //handle SIGPIPE error, just drop it and move on it's already
  //accounted for
  signal(SIGPIPE, handler);

  /****************Set up listening socket*****************/

  // Set up a socket
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if(s == -1) {
    perror("socket");
    exit(2);
  }

  // Listen at this address
  struct sockaddr_in server_addr = {
    .sin_addr.s_addr = INADDR_ANY,
    .sin_family = AF_INET,
    .sin_port = htons(SERVER_PORT)
  };

  // Bind to the specified address
  if(bind(s, (struct sockaddr*)&server_addr, sizeof(struct sockaddr_in))) {
    perror("bind");
    exit(2);
  }

  // Get the listening socket info
  socklen_t server_addr_size = sizeof(struct sockaddr_in);
  getsockname(s,(struct sockaddr *) &server_addr, &server_addr_size);

  printf("Listening on port %d\n...", SERVER_PORT);
  //Listening socket prepared, start accepting connections
  listen(s, 2);

  //repeatedly accept connections
  while(true) {

    // Accept a client connection
    struct sockaddr_in server_addr;
    socklen_t server_addr_len = sizeof(struct sockaddr_in);
    int child_socket = accept(s, (struct sockaddr*)&server_addr, &server_addr_len);

    slist_push(&g_child_socks, child_socket);

    //set up child thread to get input from this connection
    pthread_t child_thread; //TODO Fix memory leak from this-- figure out where to free it
    socket_arg_t* arg = (socket_arg_t*)malloc(sizeof(socket_arg_t));
    arg->socket = child_socket;

    if(pthread_create(&child_thread, NULL, child_thread_fn, arg)) {
      perror("Failed to create child thread");
      exit(2);
    }
  }

  return 0;
}

//arg just contains the socket number
void* child_thread_fn(void* arg) {
  int s = ((socket_arg_t*)arg)->socket;

  while(g_active) {

    uint16_t msg_len;
    if(read(s, &msg_len, 2) > 0) {
      msg_len = ntohs(msg_len);

      char* line = malloc(msg_len);
      read(s, line, msg_len);

      line[msg_len - 1] = '\0';


      //parse query and handle request appropriately
      parse_query(line, s);

      //drop that line!!
      free(line);
    }
  }

  return NULL;
}

void trim_message(char* message) {
  char* to_remove = strchr(message, '\n');
  if(to_remove != NULL) {
    *to_remove = '\0';
  }
}

//NOTE: directly modifies query, must not be string literal
void parse_query(char* query, int socket) {
  if(_DEBUG) {printf("Received query: %s\n", query); }

  //get command str and args
  char* args = strchr(query, ' ');
  if(args == NULL) {
    fprintf(stderr, "Invalid query received. Given %s\n", query);
    raise(SIGINT);
  }
  *args = '\0';
  args++;

  if(args == NULL) {
    //invalid command
    if(_DEBUG) { printf("Invalid command. Given %s\n", query); }
  } else if(strcmp(query, "set") == 0) {
    if(_DEBUG) { printf("Received set command.\n"); }
    return parse_set(args);
  } else if(strcmp(query, "add") == 0) {
    if(_DEBUG) { printf("Received add command.\n"); }
    return parse_set(args); //TODO NOTE URGENT CHANGE TO ADD
  } else if(strcmp(query, "get") == 0) {
    if(_DEBUG) { printf("Received get command.\n"); }
    return parse_get(args, socket);
  } else if(strcmp(query, "gets") == 0) {
    if(_DEBUG) { printf("Received gets command.\n"); }
    return parse_gets(args, socket);
  } else if(strcmp(query, "delete") == 0) {
    if(_DEBUG) { printf("Received delete command.\n"); }
    return parse_delete(args);
  } else {
    //unrecognized command
    if(_DEBUG) { printf("Unrecognized command: Given %s\n", query); }
  }
}

void parse_set(char* args) {
  //parse string into tokens
  char* rest = strchr(args, ' ');
  *rest = '\0';
  rest++;

  if(rest == NULL) {
    //invalid set query
    if(_DEBUG) { printf("Invalid set query. Given %s\n", args); }
    return;
  }

  //args is now the key
  char* key = args;

  //get length of data
  size_t data_length = mcache_data_len(rest);

  //allocate and fill data
  void* data = malloc(data_length);
  memcpy(data, rest, data_length);

  //if object size is greater than storage space, don't store it
  if(data_length > MCACHE_MAX_ALLOCATION) {
    return;
  }

  //evict until there's enough space new object
  while(g_memory_allocated + data_length > MCACHE_MAX_ALLOCATION) {
    //grab the last recently used object
    key_data_t* polled = klist_poll(&g_keys);

    //update memory_allocated
    g_memory_allocated -= polled->data_size;

    //remove this evicted thing from hmap
    hashmap_remove(&g_hmap, polled->key);
    if(_DEBUG) { printf("Evicted key: %s, freeing %zu bytes.\n", polled->key, polled->data_size); }

    //free stuff
    free(polled->key);
    free(polled);
  }

  //update global memory allocation
  g_memory_allocated += data_length;

  if(_DEBUG) {
    printf("Total memory allocated: %lu\n", g_memory_allocated);
    printf("Total keys stored: %zu\n", g_keys.length);
  }

  byte_sequence_t* formatted_data = (byte_sequence_t*) malloc(sizeof(byte_sequence_t));
  if(formatted_data == NULL) {
    fprintf(stderr, "Failed to allocate memory for data.\n");
    exit(EXIT_FAILURE);
  }

  formatted_data->data = data;
  formatted_data->length = data_length;

  hashmap_put(&g_hmap, key, formatted_data);

  //update keys queue
  touch_key(key, data_length);
}

void parse_add(char* args) {
  //parse string into tokens
  char* rest = strchr(args, ' ');
  *rest = '\0';
  rest++;

  if(rest == NULL) {
    //invalid set query
    if(_DEBUG) { printf("Invalid add query. Given %s\n", args); }
    return;
  }

  //args is now the key
  char* key = args;

  //get length of data
  size_t data_length = mcache_data_len(rest);

  //allocate and fill data
  void* data = malloc(data_length);
  memcpy(data, rest, data_length);

  //if object size is greater than storage space, don't store it
  if(data_length > MCACHE_MAX_ALLOCATION) {
    return;
  }

  //evict until there's enough space for new object
  while(g_memory_allocated + data_length > MCACHE_MAX_ALLOCATION) {
    //grab the last recently used object
    key_data_t* polled = klist_poll(&g_keys);

    //update memory_allocated
    g_memory_allocated -= polled->data_size;

    //remove this evicted thing from hmap
    hashmap_remove(&g_hmap, polled->key);
    if(_DEBUG) { printf("Evicted key: %s, freeing %zu bytes.\n", polled->key, polled->data_size); }

    //free stuff
    free(polled->key);
    free(polled);
  }

  //update global memory allocation
  g_memory_allocated += data_length;

  if(_DEBUG) {
    printf("Total memory allocated: %lu\n", g_memory_allocated);
    printf("Total keys stored: %zu\n", g_keys.length);
  }

  byte_sequence_t* formatted_data = (byte_sequence_t*) malloc(sizeof(byte_sequence_t));
  if(formatted_data == NULL) {
    fprintf(stderr, "Failed to allocate memory for data.\n");
    exit(EXIT_FAILURE);
  }

  formatted_data->data = data;
  formatted_data->length = data_length;

  hashmap_put(&g_hmap, key, formatted_data);

  //update keys queue
  touch_key(key, data_length);
}

//NOTE sends first 2 bytes as the length of the message
void parse_get(char* args, int socket) {
  if(_DEBUG) { printf("getting key: %s\n", args); }
  byte_sequence_t* value = hashmap_get(&g_hmap, args);

  if(value == NULL) {
    int16_t fail_msg = (int16_t)htons(-1);
    write(socket, &fail_msg, 2);
  } else {
    /*
    //NOTE TODO inefficient potentially?? have to copy data potench unnecessarily
    size_t messagelen = 2 + value->length; //2 byte size, and then pointer to the start of data strm
    void* message = malloc(messagelen); //malloc space for message
    memcpy(message, &(value->length), 2);
    memcpy(message, value->data, value->length);

    write(socket, message, messagelen);
    */
    int16_t datalen = (int16_t)htons(value->length); //NOTE TODO limited to 16 bits of length!!!!!!

    //update keys queue
    touch_key(args, value->length);

    write(socket, &datalen, 2);
    write(socket, value->data, value->length);
  }
}

void parse_gets(char* args, int socket) {
  //implement this last, this will be kind of a pain to do and not really necessary for now
  //just loop through all of the args and stick to end of byte_seq pointer array
  //byte_sequence_t** data = mcache_gets(char** keys)
  //TODO actually do this
}

void parse_delete(char* args) {
  //remove key from eviction list
  key_data_t* key_data = klist_remove(&g_keys, args);

  //key not found, nothing to remove
  if(key_data == NULL) { return; }

  //update global mem allocated
  g_memory_allocated -= key_data->data_size;

  //free necessary key_data stuff
  free(key_data->key);
  free(key_data);

  //remove key/value from hashmap
  hashmap_remove(&g_hmap, args);
}

/* Note -- could replace this with just newline character?? */
size_t mcache_data_len(char* message) {
  for(int i = 0; /* no cond */; i++) {
    if(*(uint32_t*)(message + i) == MCACHE_END_BUFF) {
      return i;
    }
  }
}

//Move a key to the back of the eviction queue
void touch_key(char* key, size_t obj_size) {
  klist_add(&g_keys, key, obj_size);
}
