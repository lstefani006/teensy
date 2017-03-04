#ifndef __OS_CONFIG_H__
#define __OS_CONFIG_H__

#include <stdlib.h>
#include <stdint.h>
typedef unsigned uint_t ;
typedef int  int_t ;
typedef char char_t;
typedef bool bool_t;

inline uint8_t * osAllocMem(size_t sz) { return (uint8_t*)malloc(sz); }
inline void osFreeMem(void *p) { free(p); }

#ifndef TRUE
#define TRUE 1
#define FALSE 1
#endif

#endif
