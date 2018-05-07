#ifndef MCACHE_TYPES_H
#define MCACHE_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// This makes the header file work for both C and C++
#ifdef __cplusplus
extern "C" {
#endif

  //special byte to specify end of message
  #define MCACHE_END_BUFF 0x0c001be9
  #define MCACHE_MAX_ALLOCATION 1000

  typedef struct byte_sequence {
    void* data; /* points to the actual byte sequence */
    size_t length; /* tells how many bytes are actually stored in this seq */
  } byte_sequence_t;

  /*for testing only, pretty dumb struct otherwise*/
  typedef struct _big_struct {
    int i1;
    int i2;
    char c;
    char str[10];
  } big_struct_t;


  // This makes the header file work for both C and C++
#ifdef __cplusplus
}
#endif

#endif
