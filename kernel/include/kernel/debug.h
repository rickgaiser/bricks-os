#ifndef KERNEL_DEBUG_H
#define KERNEL_DEBUG_H


#include "kernel/fs.h"


void printk(const char * s, ...);


// Debugging object, used by printk
extern IFileIO * pDebug;


#endif
