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


#ifndef KERNEL_RINGBUFFER_H
#define KERNEL_RINGBUFFER_H


#include "kernel/pthread_k.h"
#include "stdint.h"


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
