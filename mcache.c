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

//NOTE super gross
void mcache_set(char* key, byte_sequence_t* data) {
  int keylen = strlen(key);
  int messagelen = keylen + data->length + 6;

  //allocate memory for message ("set " + key + " " + data + newline char)
  char message[messagelen];

  //copy "set " into message -- 4 bytes total
  memcpy(message, "set ", 4);

  //copy key into message after "set ".
  memcpy(message + 4, key, keylen);

  //insert space after key
  message[keylen + 4] = ' ';

  //copy data into the rest of message
  memcpy(message + keylen + 5, data->data, data->length);
  message[keylen + data->length + 5] = '\n';

  //send message to mcache server
  write(s, message, messagelen);
}

//Note: internally equivalent to set
void mcache_add(char* key, byte_sequence_t* data) {
  int keylen = strlen(key);
  int messagelen = keylen + data->length + 6;

  //allocate memory for message ("set " + key + " " + data + newline char)
  char message[messagelen];

  //copy "add " into message -- 4 bytes total
  memcpy(message, "add ", 4);

  //copy key into message after "set ".
  memcpy(message + 4, key, keylen);

  //insert space after key
  message[keylen + 4] = ' ';

  //copy data into the rest of message
  memcpy(message + keylen + 5, data->data, data->length);
  message[keylen + data->length + 5] = '\n';

  //send message to mcache server
  write(s, message, messagelen);
}

//returns NULL on failure
byte_sequence_t* mcache_get(char* key) {
  int keylen = strlen(key);
  int messagelen = keylen + 5;

  //allocate memory for message ("get " + key + newline char)
  char message[messagelen];

  //copy "get " into message -- 4 bytes total
  memcpy(message, "get ", 4);

  //copy key after "get "
  memcpy(message + 4, key, keylen);

  //replace null terminator with newline character
  message[keylen + 4] = '\n';

  //send message to mcache server
  write(s, message, messagelen);

  //get the length of the byte stream
  int16_t data_len;
  read(s, &data_len, 2);
  data_len = ntohs(data_len);
  
  //if get failed
  if(data_len == -1) {
    return NULL;
  }

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
  int keylen = strlen(key);
  int messagelen = keylen + 8;

  //allocate memory for message ("delete " + key + newline char)
  char message[messagelen];

  //copy "get " into message -- 7 bytes total
  memcpy(message, "delete ", 7);

  //copy key after "get "
  memcpy(message + 7, key, keylen);

  //replace null terminator with newline character
  message[keylen + 7] = '\n';

  //send message to mcache server
  write(s, message, messagelen);
}

void mcache_exit(void) {
  close(s);
}
