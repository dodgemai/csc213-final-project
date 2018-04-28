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
#include "socket_list.h"
#include "server.h"
#include "mcache_types.h"
#include <pthread.h>
#include <signal.h>
#include <stdint.h>

#define _DEBUG true
/* NOTE: maybe don't actually need child_socks -- we will see.
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
* parse query given by client, and handle appropriately
*/
bool parse_query(char* query);
bool parse_set(char* args);
bool parse_add(char* args);
bool parse_get(char* args);
bool parse_gets(char* args);
bool parse_delete(char* args);

//handle dumb SIGPIPE signal
void handler(int s) { }

//global values
socket_list_t child_socks;
bool active;

typedef struct socket_arg {
  int socket;
} socket_arg_t;

int main(void) {

  slist_init(&child_socks); //prep child_socks
  active = true; //set server to active

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

  // Listen at this address. We'll bind to port 0 to accept any available port
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

    slist_push(&child_socks, child_socket);

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

  // Duplicate the socket_fd so we can open it twice, once for input and once for output
  int socket_fd_copy = dup(s);
  if(socket_fd_copy == -1) {
    return NULL;
  }

  // Open the socket as a FILE stream so we can use fgets
  FILE* input = fdopen(socket_fd_copy, "r");
  // Check for errors
  if(input == NULL) {
    return NULL;
  }

  while(active) {

    char* line = NULL;
    size_t linecap = 0;
    if(getline(&line, &linecap, input) == -1) {
      return NULL;
    }

    parse_query(line);
    free(line);
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
bool parse_query(char* query) {
  char* args = strtok(query, " ");

  if(args == NULL) {
    //invalid command
    if(_DEBUG) { printf("Invalid command. Given %s\n", query); }
    return false;
  } else if(strcmp(query, "set") == 0) {
    if(_DEBUG) { printf("Received set command.\n"); }
    return parse_set(args);
  } else if(strcmp(query, "add") == 0) {
    if(_DEBUG) { printf("Received add command.\n"); }
    return parse_add(args);
  } else if(strcmp(query, "get") == 0) {
    if(_DEBUG) { printf("Received get command.\n"); }
    return parse_get(args);
  } else if(strcmp(query, "gets") == 0) {
    if(_DEBUG) { printf("Received gets command.\n"); }
    return parse_gets(args);
  } else if(strcmp(query, "delete") == 0) {
    if(_DEBUG) { printf("Received delete command.\n"); }
    return parse_delete(args);
  } else {
    //unrecognized command
    if(_DEBUG) { printf("Unrecognized command.\n"); }
    return false;
  }
}

bool parse_set(char* args) {
  char* rest = strtok(args, " ");
  if(rest == NULL) {
    //invalid set query
    if(_DEBUG) { printf("Invalid set query. Given %s\n", args); }
    return false;
  }

  //args is now the key
  char* key = args;
  uint8_t* data = (uint8_t*)rest;
  int data_length = strlen(rest);
  byte_sequence_t* formatted_data = (byte_sequence_t*) malloc(sizeof(byte_sequence_t));
  if(formatted_data == NULL) {
    fprintf(stderr, "Failed to allocate memory for data.\n");
    exit(EXIT_FAILURE);
  }

  formatted_data->data = data;
  formatted_data->length = data_length;

  //mcache_set(key, formatted_data); //TODO implement and uncomment
  return true; //TODO fix
}

bool parse_add(char* args) {
  char* rest = strtok(args, " ");
  if(rest == NULL) {
    //invalid set query
    if(_DEBUG) { printf("Invalid set query. Given %s\n", args); }
    return false;
  }

  //args is now the key
  char* key = args;
  uint8_t* data = (uint8_t*)rest;
  int data_length = strlen(rest);
  byte_sequence_t* formatted_data = (byte_sequence_t*) malloc(sizeof(byte_sequence_t));
  if(formatted_data == NULL) {
    fprintf(stderr, "Failed to allocate memory for data.\n");
    exit(EXIT_FAILURE);
  }

  formatted_data->data = data;
  formatted_data->length = data_length;

  //mcache_add(key, formatted_data); //TODO implement and uncomment
  return true; //TODO fix
}

bool parse_get(char* args) {
  //byte_sequence_t* data = mcache_get(char* key);
  //TODO send data to baby, may have to rethink return value??
  return true; //TODO fix
}

bool parse_gets(char* args) {
  //implement this last, this will be kind of a pain to do and not really necessary for now
  //just loop through all of the args and stick to end of byte_seq pointer array
  //byte_sequence_t** data = mcache_gets(char** keys)
  return true; //TODO fix
}

bool parse_delete(char* args) {
  //mcache_delete(args); //TODO implement and uncomment
  return true; //TODO fix
}
