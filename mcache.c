#ifndef MCACHE_H
#define MCACHE_H

#include <stdbool.h>
#include <entry_list.h>
#include <hashmap.h>

bool mcache_set(hashmap_t* map, char* key, byte_sequence_t* data){
  hashmap_put(map, key, data);
}

bool mcache_add(hashmap_t* map, char* key, byte_sequence_t* data){
  hashmap_put(map, key, data);
}

byte_sequence_t* mcache_get(hashmap_t* map, char* key){
  hashmap_get(map, key);
}

byte_sequence_t** mcache_gets(hashmap_t* map, char** keys, size_t num_keys){
  //TODO: add this thing
}

bool mcache_delete(hashmap_t* map, char* key){
  hashmap_remove(map, key);
}
