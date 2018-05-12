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

#define IN_CLIENT false

int s;
size_t g_cache_misses;

// Initialize the mcache server
void mcache_init(char* server_address) {
  g_cache_misses = 0;
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

// Adds data into the mcache -- if the key already exists, update value
void mcache_set(char* key, void* data_ptr, size_t num_bytes) {
  int keylen = strlen(key);
  int messagelen = keylen + num_bytes + 6;
  //delimiter = "0x0c001be9"
  //allocate memory for message ("set " + key + " " + data + delimiting 4byte + newline char)
  char message[messagelen];

  //copy "set " into message -- 4 bytes total
  memcpy(message, "set ", 4);

  //copy key into message after "set ".
  memcpy(message + 4, key, keylen);

  //insert space after key
  message[keylen + 4] = ' ';

  //uint32_t delimiter = MCACHE_END_BUFF;
  //copy data into the rest of message
  memcpy(message + keylen + 5, data_ptr, num_bytes);
  //memcpy(message + keylen + num_bytes + 5, &delimiter, 4);
  message[keylen + num_bytes + 5] = '\n';

  uint16_t message_size = (uint16_t)htons(messagelen);
  write(s, &message_size, 2); //write the data size to the server
  //send message to mcache server
  write(s, message, messagelen); //write the actual message to the server

  //write the data size to server
  message_size = (uint16_t)htons(num_bytes);
  write(s, &message_size, 2);
}

//Note: internally equivalent to set
void mcache_add(char* key, void* data_ptr, size_t num_bytes) {
  int keylen = strlen(key);
  int messagelen = keylen + num_bytes + 6;
  //delimiter = "0x0c001be9"
  //allocate memory for message ("set " + key + " " + data + delimiting 4byte + newline char)
  char message[messagelen];

  //copy "add " into message -- 4 bytes total
  memcpy(message, "add ", 4);

  //copy key into message after "set ".
  memcpy(message + 4, key, keylen);

  //insert space after key
  message[keylen + 4] = ' ';

  //uint32_t delimiter = MCACHE_END_BUFF;
  //copy data into the rest of message
  memcpy(message + keylen + 5, data_ptr, num_bytes);
  //memcpy(message + keylen + num_bytes + 5, &delimiter, 4);
  message[keylen + num_bytes + 5] = '\n';

  uint16_t message_size = (uint16_t)htons(messagelen);
  write(s, &message_size, 2); //write the data size to the server

  //send message to mcache server
  write(s, message, messagelen);

  //write the data size to server
  message_size = (uint16_t)htons(num_bytes);
  write(s, &message_size, 2);
}

// Gets a value from the mcache by key
//returns NULL on failure
//key is key to value
//NOTE: returned value must be freed by user
void* mcache_get(char* key) {
  int keylen = strlen(key);
  int messagelen = keylen + 5;

  //allocate memory for message ("get " + key + null byte)
  char message[messagelen];

  //copy "get " into message -- 4 bytes total
  memcpy(message, "get ", 4);

  //copy key after "get "
  memcpy(message + 4, key, keylen);

  //replace null terminator with newline character
  message[keylen + 4] = '\n';

  uint16_t message_size = (uint16_t)htons(messagelen);
  write(s, &message_size, 2); //write the data size to the server

  //send message to mcache server
  write(s, message, messagelen);

  //get the length of the byte stream
  int16_t data_len;
  read(s, &data_len, 2);
  data_len = ntohs(data_len);

  //if get failed
  if(data_len == -1) {
    g_cache_misses++;
    return NULL;
  }

  void* ret = malloc(data_len);
  read(s, ret, data_len);

  return ret;
}

// Gets an array of values from mcache by an array of keys
byte_sequence_t** mcache_gets(char** keys, size_t num_keys) {
  //TODO: add this thing
  return NULL;
}

// Deletes data stored in the mcache by key
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

  uint16_t message_size = (uint16_t)htons(messagelen);
  write(s, &message_size, 2); //write the data size to the server

  //send message to mcache server
  write(s, message, messagelen);
}

size_t mcache_get_cache_misses() {
  return g_cache_misses;
}

// Close the mcache server
void mcache_exit(void) {
  close(s);
}
