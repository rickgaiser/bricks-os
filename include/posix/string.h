#ifndef STRING_H
#define STRING_H


#include "stddef.h"


#ifdef __cplusplus
extern "C" {
#endif


int    strcmp(const char * s1, const char * s2);
size_t strlen(const char * s);
char * strcpy(char * s1, const char * s2);
char * strncpy(char * s1, const char * s2, size_t n);
void * memset(void *, int, size_t);
void * memcpy(void * s1, const void * s2, size_t n);


#ifdef __cplusplus
}
#endif


#endif
