#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H


#include "stddef.h"


#ifdef __cplusplus
extern "C" {
#endif


void init_heap(void * start, size_t size);

char *sbrk(int incr);
void * kmalloc(size_t size);
void kfree(void * block);


#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
void * operator new(size_t size);
void operator delete(void * mem);
#endif


#endif
