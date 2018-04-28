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
#include <pthread.h>
#include <signal.h>

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
void parse_query(char* query);

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
void parse_query(char* query) {
  char* args = strtok(query, " ");

  if(args == NULL) {
    //invalid command
    if(_DEBUG) { printf("Invalid command. Given %s\n", query); }
    return;
  }

  if(strcmp(query, "set") == 0) {
    if(_DEBUG) { printf("Received set command.\n"); }
    //TODO do set stuff
  } else if(strcmp(query, "add") == 0) {
    if(_DEBUG) { printf("Received add command.\n"); }
    //TODO do add stuff
  } else if(strcmp(query, "get") == 0) {
    if(_DEBUG) { printf("Received get command.\n"); }
    //TODO do get stuff
  } else if(strcmp(query, "gets") == 0) {
    if(_DEBUG) { printf("Received gets command.\n"); }
    //TODO do gets stuff
  } else if(strcmp(query, "delete") == 0) {
    if(_DEBUG) { printf("Received delete command.\n"); }
    //TODO do delete stuff
  }
}
