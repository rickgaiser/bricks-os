#ifndef STRING_H
#define STRING_H


#include "stddef.h"


#ifdef __cplusplus
extern "C" {
#endif


int    strcmp(const char * s1, const char * s2);
size_t strlen(const char * s);
void * memset(void *, int, size_t);


#ifdef __cplusplus
}
#endif


#endif
