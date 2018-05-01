#include "hashmap.h"
#include "entry_list.h"
#include "mcache_types.h"
#include <stdio.h>

// Initialize a hashmap
void hashmap_init(hashmap_t* map) {
  map->capacity = INITIAL_CAPACITY;
  map->size_used = 0;
  pthread_mutex_init(&(map->map_lock), NULL);
  map->table = (bucket_t**) calloc(INITIAL_CAPACITY, sizeof(entry_list_t*));
  if(map->table == NULL) {
    fprintf(stderr, "Failed to allocate memory for hashmap.\n");
    exit(EXIT_FAILURE);
  }
}

bucket_t* bucket_init(void) {
  bucket_t* ret = (bucket_t*) malloc(sizeof(bucket_t));
  if(ret == NULL) {
    fprintf(stderr, "Failed to allocate space for bucket.\n");
    exit(EXIT_FAILURE);
  }
  elist_init(ret->elist);
  pthread_mutex_init(&ret->m, NULL);
  return ret;
}

/**
* djb2 algorithm
* Credit to Dan Bernstein:
* www.cse.yorku.ca/~oz/hash.html
*/
unsigned long hash(char* str) {
  unsigned long hash = 5381;
  int c;

  while((c = *str++)) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c*/
  }
  return hash;
}

void hashmap_bucket_destroy(bucket_t* bucket) {
  elist_destroy(bucket->elist);
  pthread_mutex_destroy(&bucket->m);
}

// Destroy a hashmap
void hashmap_destroy(hashmap_t* map) {
  if(map == NULL) { return; }
  size_t touched = 0;

  for(int i = 0; i < map->capacity && touched < map->size_used; i++) {
    bucket_t* cur = *(map->table + i);
    if(cur != NULL) {
      //destroy bucket, and NULL out entry
      hashmap_bucket_destroy(cur);
      *(map->table + i) = NULL;
      touched++;
    }
  }
  pthread_mutex_destroy(&map->map_lock);
}

// Check if hashmap contains key
bool hashmap_contains_key(hashmap_t* map, char* key) {
  if(map == NULL) { return false; }

  unsigned int index = hash(key) % map->capacity;
  return *(map->table + index) == NULL;
}

// Get a value by key
void* hashmap_get(hashmap_t* map, char* key) {
  if(map == NULL) { return NULL; }

  unsigned int index = hash(key) % map->capacity;
  bucket_t* bucket = *(map->table + index);

  //if no entry, return null
  if(bucket == NULL) { return NULL; }

  //get value
  pthread_mutex_lock(&bucket->m);
  byte_sequence_t* data = elist_get(bucket->elist, key);
  pthread_mutex_unlock(&bucket->m);

  return data;
}

// Get the key set for given map
char** hashmap_key_set(hashmap_t* map) {
  /*TODO implement if necessary, maybe not necessary though...
  really only necessary for debugging purposes, client no use!
  could implement by just having a list of valid indices *shrug* tied to
  the hashmap */
  return NULL;
}

// Determine whether or not map is empty
bool hashmap_is_empty(hashmap_t* map) {
  if(map == NULL) { return false; }
  return map->size_used == 0;
}

// Determine the size of the map
int hashmap_size(hashmap_t* map) {
  if(map == NULL) { return -1; }
  return map->size_used;
}

// Put element into map -- if key already exists, update value
void hashmap_put(hashmap_t* map, char* key, byte_sequence_t* value) {
  if(map == NULL) { return; }
  unsigned int index = hash(key) % map->capacity;
  bucket_t* _bucket = *(map->table + index);

  //if bucket doesn't exist, make one and add key-value pair
  if(_bucket == NULL) {
    *(map->table + index) = bucket_init();
  }

  pthread_mutex_lock(&_bucket->m);
  //if bucket already exists, add key-value pair
  elist_push_unique(_bucket->elist, key, value);
  pthread_mutex_unlock(&_bucket->m);
}

// Remove an element from map
void hashmap_remove(hashmap_t* map, char* key) {
  if(map == NULL) { return; }
  unsigned int index = hash(key) % map->capacity;
  bucket_t* bucket = *(map->table + index);

  //if bucket doesn't exist, exit
  if(bucket == NULL) { return; }

  //remove key
  pthread_mutex_lock(&bucket->m);
  elist_remove(bucket->elist, key);
  pthread_mutex_unlock(&bucket->m);

  //if no more keys in bucket, destroy and zero out
  if(bucket->elist->length == 0) {
    hashmap_bucket_destroy(bucket);
    map->size_used--;
    *(map->table + index) = NULL;
  }
}
