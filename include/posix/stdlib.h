#ifndef STDLIB_H
#define STDLIB_H


#ifdef __cplusplus
extern "C" {
#endif


#include "stddef.h"


//inline int abs(int n){return (n >= 0 ? n : -n);}
#define abs(n) (n >= 0 ? n : -n)
void free(void *);
void * malloc(size_t);


#ifdef __cplusplus
}
#endif


#endif
