#ifndef STDLIB_H
#define STDLIB_H


#ifdef __cplusplus
extern "C" {
#endif


#include "stddef.h"


void free(void *);
void * malloc(size_t);


#ifdef __cplusplus
}
#endif


#endif
