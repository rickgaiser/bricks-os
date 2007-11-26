#ifndef KERNEL_DEBUG_H
#define KERNEL_DEBUG_H


#ifdef __cplusplus
#include "kernel/fs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


void printk(const char * fmt, ...);
void panic(const char * fmt, ...);


#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
// Debugging object, used by printk
extern IFileIO * pDebug;
#endif


#endif
