/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#include "kernel/ringBuffer.h"


// -----------------------------------------------------------------------------
CRingBuffer::CRingBuffer()
 : inPtr_(0)
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
bool
CRingBuffer::put(uint8_t data, bool block)
{
  //printk("%c", (char)data);

  // Calculate new inPtr_
  uint32_t in = inPtr_ + 1;
  if(in >= RINGBUFFER_SIZE)
    in = 0;

  if(block == true)
  {
    // Wait for data in buffer
    while(in == outPtr_)
      k_pthread_mutex_lock(&mutex_);
  }
  else
  {
    // Buffer full
    if(in == outPtr_)
      return false;
  }

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
  if(outPtr_ >= (RINGBUFFER_SIZE - 1))
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
