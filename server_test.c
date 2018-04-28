#include "server.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int connect_to_server(char* server_address, int client_port) {
  struct hostent* server = gethostbyname(server_address);
  if(server == NULL) {
    fprintf(stderr, "Unable to find host %s\n", server_address);
    exit(1);
  }

  int s = socket(AF_INET, SOCK_STREAM, 0);
  if(s == -1) {
    perror("socket failed");
    exit(2);
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(SERVER_PORT);

  bcopy((char*)server->h_addr, (char*)&addr.sin_addr.s_addr, server->h_length);

  if(connect(s, (struct sockaddr*)&addr, sizeof(struct sockaddr_in))) {
    perror("connect failed");
    exit(2);
  }

  return s;
}

int main(void) {
  int s = connect_to_server("localhost", SERVER_PORT);

  // Duplicate the socket_fd so we can open it twice, once for input and once for output
  int socket_fd_copy = dup(s);
  if(socket_fd_copy == -1) {
    perror("dup failed");
    exit(EXIT_FAILURE);
  }

  // Open the socket as a FILE stream so we can use fgets
  FILE* input = fdopen(s, "r");
  FILE* output = fdopen(socket_fd_copy, "w");

  // Check for errors
  if(input == NULL || output == NULL) {
    perror("fdopen failed");
    exit(EXIT_FAILURE);
  }

  //send get query
  fprintf(output, "get %d\n", s);
  fflush(output);

  /*
  //get uid
  char* str_uid = NULL;
  size_t linecap = 0;
  getline(&str_uid, &linecap, input);

  int uid = atoi(str_uid);
  */

  close(s);
}
