#ifndef MCACHE_TYPES_H
#define MCACHE_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// This makes the header file work for both C and C++
#ifdef __cplusplus
extern "C" {
#endif

  typedef struct byte_sequence {
    uint8_t* data; /* points to the actual byte sequence */
    size_t length; /* tells how many bytes are actually stored in this seq */
  } byte_sequence_t;

  // This makes the header file work for both C and C++
#ifdef __cplusplus
}
#endif

#endif
