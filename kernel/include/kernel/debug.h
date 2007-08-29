#ifndef KERNEL_DEBUG_H
#define KERNEL_DEBUG_H


#include "kernel/fs.h"


#define va_list         __builtin_va_list
#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)


void printk(const char * fmt, ...);
void panic(const char * fmt, ...);


// Debugging object, used by printk
extern IFileIO * pDebug;


#endif
