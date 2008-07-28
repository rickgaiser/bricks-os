#include "kernel/ringBuffer.h"


// -----------------------------------------------------------------------------
CRingBuffer::CRingBuffer()
 : size_(128)
 , inPtr_(0)
 , outPtr_(0)
{
  // Initialize mutex to a locked state so we can use it to wait for data
  k_pthread_mutex_init(&mutex_, NULL);
  k_pthread_mutex_lock(&mutex_);
}

// -----------------------------------------------------------------------------
CRingBuffer::~CRingBuffer()
{
}

// -----------------------------------------------------------------------------
// CAN NOT BLOCK, is used from ISR
bool
CRingBuffer::put(uint8_t data)
{
  //printk("%c", (char)data);

  // Calculate new inPtr_
  uint32_t in = inPtr_ + 1;
  if(in >= size_)
    in = 0;

  // Buffer full
  if(in == outPtr_)
    return false;

  // Place data
  buffer_[inPtr_] = data;

  // Increment inPtr_
  inPtr_ = in;

  return true;
}

// -----------------------------------------------------------------------------
bool
CRingBuffer::get(uint8_t * data, bool block)
{
  if(block == true)
  {
    // Wait for data in buffer
    while(inPtr_ == outPtr_)
      k_pthread_mutex_lock(&mutex_);
  }
  else
  {
    // Check empty buffer
    if(inPtr_ == outPtr_)
      return false;
  }

  // Get data
  *data = buffer_[outPtr_];

  // Increment outPtr_
  if(outPtr_ >= (size_ - 1))
    outPtr_ = 0;
  else
    outPtr_++;

  return true;
}

// -----------------------------------------------------------------------------
void
CRingBuffer::notifyGetters()
{
  // Unlock threads waiting for data
  k_pthread_mutex_unlock(&mutex_);
}
