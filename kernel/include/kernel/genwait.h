#ifndef GENWAIT_H
#define GENWAIT_H


#include "inttypes.h"


int genwait_wait(void * obj, useconds_t useconds);
int genwait_wake(void * obj, int maxcount);


#endif
