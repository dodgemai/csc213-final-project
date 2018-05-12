/*
* This is an implementation of mcache that will work within client process
* rather than in a separate network process. This will theoretically be faster
* because no need to send data over TCP connection
*/

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
#include <pthread.h>

#include "server.h"
#include "mcache.h"
#include "hashmap.h"
#include "socket_list.h"
#include "key_list.h"
#include "mcache_types.h"

#define IN_CLIENT true

//global values
hashmap_t g_hmap;
key_list_t g_keys;
size_t g_memory_allocated;
pthread_mutex_t g_m;
size_t g_cache_misses;

//for passing key_data to update_keys_fn
typedef struct key_data_arg {
  char* key;
  size_t obj_size;
} key_data_arg_t;

//move key to the back of the eviction queue
void* update_keys_fn(void* arg) {
  char* key = ((key_data_arg_t*)arg)->key;
  klist_add(&g_keys, key, ((key_data_arg_t*)arg)->obj_size);
  free(key);
  return NULL;
}

//Move a key to the back of the eviction queue (in child thread)
void touch_key(char* key, size_t obj_size) {
  klist_add(&g_keys, key, obj_size);
  /*
  //get a copy of key for thread safety stuff
  key = strdup(key);

  //set up child thread to get input from this connection
  pthread_t t;

  //set up arg
  key_data_arg_t* arg = (key_data_arg_t*)malloc(sizeof(key_data_arg_t));
  if(arg == NULL) {
  fprintf(stderr, "Failed to allocate memory for key_data_arg\n");
  exit(EXIT_FAILURE);
}
arg->key = key;
arg->obj_size = obj_size;

//create thread
if(pthread_create(&t, NULL, update_keys_fn, arg)) {
perror("Failed to create thread for update_keys_fn\n");
exit(EXIT_FAILURE);
}
pthread_detach(t);
*/
}

// Initialize the mcache server
void mcache_init(char* server_address) {
  hashmap_init(&g_hmap); //prep g_hmap
  klist_init(&g_keys);  //prep g_keys
  g_memory_allocated = 0; //init mem_allocated
  g_cache_misses = 0;
  pthread_mutex_init(&g_m, NULL);
}

// Adds data into the mcache -- if the key already exists, update value
void mcache_set(char* key, void* data_ptr, size_t num_bytes) {
  //if object size is greater than storage space, don't store it
  if(num_bytes > MCACHE_MAX_ALLOCATION) {
    return;
  }

  //evict until there's enough space for new object
  while(g_memory_allocated + num_bytes > MCACHE_MAX_ALLOCATION) {
    //grab the last recently used object
    key_data_t* polled = klist_poll(&g_keys);

    pthread_mutex_lock(&g_m);
    //update memory_allocated
    g_memory_allocated -= polled->data_size;

    //remove from hashmap
    hashmap_remove(&g_hmap, polled->key);
    pthread_mutex_unlock(&g_m);

    //free stuff
    free(polled->key);
    free(polled);
  }


  //update global memory allocation
  pthread_mutex_lock(&g_m);
  g_memory_allocated += num_bytes;
  pthread_mutex_unlock(&g_m);

  byte_sequence_t* formatted_data = (byte_sequence_t*) malloc(sizeof(byte_sequence_t));
  if(formatted_data == NULL) {
    fprintf(stderr, "Failed to allocate memory for data.\n");
    exit(EXIT_FAILURE);
  }

  //copy data_ptr value for cache storage
  void* data = malloc(num_bytes);
  if(data == NULL) {
    fprintf(stderr, "failed to allocate memory for data\n");
    exit(EXIT_FAILURE);
  }
  memcpy(data, data_ptr, num_bytes);

  formatted_data->data = data;
  formatted_data->length = num_bytes;

  hashmap_put(&g_hmap, key, formatted_data);

  //update keys queue
  touch_key(key, num_bytes);
}


void mcache_add(char* key, void* data_ptr, size_t num_bytes) {
  //if object size is greater than storage space, don't store it
  if(num_bytes > MCACHE_MAX_ALLOCATION) {
    return;
  }

  //evict until there's enough space for new object
  while(g_memory_allocated + num_bytes > MCACHE_MAX_ALLOCATION) {
    //grab the last recently used object
    key_data_t* polled = klist_poll(&g_keys);
    if(polled == NULL) { continue; }
    //update memory_allocated
    pthread_mutex_lock(&g_m);
    g_memory_allocated -= polled->data_size;

    //remove from hashmap
    hashmap_remove(&g_hmap, polled->key);
    pthread_mutex_unlock(&g_m);

    //free stuff
    free(polled->key);
    free(polled);
  }

  //update global memory allocation
  pthread_mutex_lock(&g_m);
  g_memory_allocated += num_bytes;
  pthread_mutex_unlock(&g_m);

  byte_sequence_t* formatted_data = (byte_sequence_t*) malloc(sizeof(byte_sequence_t));
  if(formatted_data == NULL) {
    fprintf(stderr, "Failed to allocate memory for data.\n");
    exit(EXIT_FAILURE);
  }

  //copy data_ptr value for cache storage
  void* data = malloc(num_bytes);
  if(data == NULL) {
    fprintf(stderr, "failed to allocate memory for data\n");
    exit(EXIT_FAILURE);
  }
  memcpy(data, data_ptr, num_bytes);

  formatted_data->data = data;
  formatted_data->length = num_bytes;

  hashmap_offer(&g_hmap, key, formatted_data);

  //update keys queue
  touch_key(key, num_bytes);
}

// Gets a value from the mcache by key
//returns NULL on failure
//key is key to value
//NOTE: returned value must be freed by user
void* mcache_get(char* key) {
  printf("Keyset: \n");
  key_node_t* cur = g_keys.first;
  for(int i = 0; i < g_keys.length; i++) {
    printf("key: %d\n", *(int*)cur->data->key);
    cur = cur->next;
  }

  //get value
  byte_sequence_t* value = hashmap_get(&g_hmap, key);

  //cache miss, return NULL
  if(value == NULL) {
    g_cache_misses++;
    return NULL;
  }

  //touch key to denote usage
  touch_key(key, value->length);

  //allocate space for ret
  void* ret = malloc(value->length);
  if(ret == NULL) { perror("malloc mcache_get"); exit(EXIT_FAILURE);}
  //fill in ret with appropriate value
  memcpy(ret, value->data, value->length);

  return ret;
}

// Gets an array of values from mcache by an array of keys
byte_sequence_t** mcache_gets(char** keys, size_t num_keys) {
  //TODO: add this thing
  return NULL;
}

// Deletes data stored in the mcache by key
void mcache_delete(char* key) {
  //remove key from eviction list
  key_data_t* key_data = klist_remove(&g_keys, key);

  //key not found, nothing to remove
  if(key_data == NULL) { return; }

  //update global mem allocated
  pthread_mutex_lock(&g_m);
  g_memory_allocated -= key_data->data_size;
  pthread_mutex_unlock(&g_m);

  //free necessary key_data stuff
  free(key_data->key);
  free(key_data);

  //remove key/value from hashmap
  hashmap_remove(&g_hmap, key);
}
size_t mcache_get_cache_misses() {
  return g_cache_misses;
}
// Cleanup
void mcache_exit(void) {
  hashmap_destroy(&g_hmap);
  klist_destroy(&g_keys);
  pthread_mutex_destroy(&g_m);
}
