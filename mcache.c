#include <stdbool.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>

#include "entry_list.h"
#include "server.h"
#include "mcache.h"

int s;

void mcache_init(char* server_address) {
  struct hostent* server = gethostbyname(server_address);

  if(server == NULL) {
    fprintf(stderr, "Unable to find host %s\n", server_address);
    exit(1);
  }

  s = socket(AF_INET, SOCK_STREAM, 0);
  if(s == -1) {
    perror("socket failed");
    exit(2);
  }

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(SERVER_PORT);

  bcopy((char*)server->h_addr, (char*)&addr.sin_addr.s_addr, server->h_length);

  if(connect(s, (struct sockaddr*)&addr, sizeof(struct sockaddr_in))) {
    perror("failed to connect to mcache server");
    exit(2);
  }
}
void mcache_set(char* key, byte_sequence_t* data) {

  //TODO compact into a single write for efficieny's sake!
  write(s, "set ", 4);

  int keylen = strlen(key);
  //allocate memory for message (key + " " + data + newline char)
  char message[keylen + data->length + 2];

  //copy key into message
  memcpy(message, key, keylen);

  //insert space into message
  message[keylen] = ' ';

  //copy data into the rest of message
  memcpy(message + keylen + 1, data->data, data->length);
  message[keylen + data->length] = '\n';

  //send message to mcache server
  write(s, message, keylen + data->length + 2);
}

void mcache_add(char* key, byte_sequence_t* data) {

  //TODO compact into a single write for efficieny's sake!
  write(s, "add ", 4);

  int keylen = strlen(key);
  //allocate memory for message (key + " " + data + newline char)
  char message[keylen + data->length + 2];

  //copy key into message
  memcpy(message, key, keylen);

  //insert space into message
  message[keylen] = ' ';

  //copy data into the rest of message
  memcpy(message + keylen, data->data, data->length);
  message[keylen + data->length + 1] = '\n';

  //send message to mcache server
  write(s, message, keylen + data->length + 2);
}

byte_sequence_t* mcache_get(char* key) {
  //make internal copy of key to mess with
  char* _key = strdup(key);
  int keylen = strlen(key);
  //replace null terminator with null character
  _key[keylen - 1] = '\n';

  //TODO compact into a single write for efficieny's sake!

  write(s, "get ", 4);

  //send message to mcache server
  write(s, _key, keylen);
  free(_key);

  //get the length of the byte stream
  uint16_t data_len;
  read(s, &data_len, 2);

  uint8_t byte_seq[data_len];
  read(s, byte_seq, data_len);

  //format data and fill in values
  byte_sequence_t* ret = (byte_sequence_t*) malloc(sizeof(byte_sequence_t));
  ret->data = byte_seq;
  ret->length = data_len;
  return ret;
}

byte_sequence_t** mcache_gets(char** keys, size_t num_keys) {
  //TODO: add this thing
  return NULL;
}

void mcache_delete(char* key) {
  //make internal copy of key to mess with
  char* _key = strdup(key);
  int keylen = strlen(key);
  //replace null terminator with null character
  _key[keylen - 1] = '\n';

  //TODO compact into a single write for efficieny's sake!
  write(s, "remove ", 7);

  //send message to mcache server
  write(s, _key, keylen);
  free(_key);
}

void mcache_exit(void) {
  close(s);
}
