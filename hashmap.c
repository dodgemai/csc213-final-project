#include "hashmap.h"
#include "entry_list.h"
#include "mcache_types.h"
#include <stdio.h>

  // Initialize a hashmap
void hashmap_init(hashmap_t* map) {
  map->capacity = INITIAL_CAPACITY;
  map->size_used = 0;
  map->table = (entry_list_t**) calloc(INITIAL_CAPACITY, sizeof(entry_list_t*));
  if(map->table == NULL) {
    fprintf(stderr, "Failed to allocate memory for hashmap.\n");
    exit(EXIT_FAILURE);
  }
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

// Destroy a bucket //TODO this might be broken??
void hashmap_bucket_destroy(entry_list_t* bucket) {
  elist_destroy(bucket);
}

// Destroy a hashmap
void hashmap_destroy(hashmap_t* map) {
  size_t touched = 0;

  for(int i = 0; i < map->capacity && touched < map->size_used; i++) {
    entry_list_t* cur = *(map->table + i);
    if(cur != NULL) {
      //destroy bucket, and NULL out entry
      hashmap_bucket_destroy(cur);
      *(map->table + i) = NULL;
      touched++;
    }
  }
}

// Check if hashmap contains key
bool hashmap_contains_key(hashmap_t* map, char* key) {
  unsigned int index = hash(key) % map->capacity;
  return *(map->table + index) == NULL;
}

// Get a value by key
void* hashmap_get(hashmap_t* map, char* key) {
  unsigned int index = hash(key) % map->capacity;
  entry_list_t* bucket = *(map->table + index);

  //if no entry, return null
  if(bucket == NULL) { return NULL; }

  //get value
  return elist_get(bucket, key);
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
  return map->size_used == 0;
}

// Determine the size of the map
int hashmap_size(hashmap_t* map) {
  return map->size_used;
}

// Put element into map -- if key already exists, update value
void hashmap_put(hashmap_t* map, char* key, byte_sequence_t* value) {
  unsigned int index = hash(key) % map->capacity;
  entry_list_t* bucket = *(map->table + index);

  //if bucket doesn't exist, make one and add key-value pair
  if(bucket == NULL) {
    *(map->table + index) = (entry_list_t*) malloc(sizeof(entry_list_t));
    if(*(map->table + index) == NULL) {
      fprintf(stderr, "Failed to allocate space for bucket.\n");
      exit(EXIT_FAILURE);
    }
    elist_init(*(map->table + index));
    elist_push_unique(*(map->table + index), key, value);
    return;
  }

  //if bucket already exists, add key-value pair
  else {
    elist_push_unique(bucket, key, value);
  }
}

// Remove an element from map
void hashmap_remove(hashmap_t* map, char* key) {
  unsigned int index = hash(key) % map->capacity;
  entry_list_t* bucket = *(map->table + index);

  //if bucket doesn't exist, exit
  if(bucket == NULL) { return; }

  //remove key
  elist_remove(bucket, key);

  //if no more keys in bucket, destroy and zero out
  if(bucket->length == 0) {
    hashmap_bucket_destroy(bucket);
    *(map->table + index) = NULL;
  }
}
