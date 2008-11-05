#ifndef KERNEL_RINGBUFFER_H
#define KERNEL_RINGBUFFER_H


#include "kernel/pthread_k.h"
#include "inttypes.h"


#define RINGBUFFER_SIZE 512


// -----------------------------------------------------------------------------
class CRingBuffer
{
public:
  CRingBuffer();
  ~CRingBuffer();

  bool put(uint8_t data, bool block = false);
  bool get(uint8_t * data, bool block = false);
  void notifyGetters();

private:
  uint8_t buffer_[RINGBUFFER_SIZE];
  uint32_t size_;
  volatile uint32_t inPtr_;
  volatile uint32_t outPtr_;
  pthread_mutex_t mutex_;
};


#endif
