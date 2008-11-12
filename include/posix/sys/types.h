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


#ifndef TYPES_H
#define TYPES_H


typedef unsigned int     clock_t;
typedef long long int    loff_t;
typedef long             off_t;
typedef int              pid_t;

typedef struct pthread_attr_t
{
} pthread_attr_t;

typedef struct pthread_cond_t
{
  volatile int iLock;
} pthread_cond_t;

typedef struct pthread_condattr_t
{
} pthread_condattr_t;

typedef struct pthread_mutex_t
{
  volatile int iLock;
} pthread_mutex_t;

typedef struct pthread_mutexattr_t
{
} pthread_mutexattr_t;

typedef struct pthread_t
{
  void * pThread;
} pthread_t;

#ifndef __SIZE_T__
#define __SIZE_T__
typedef unsigned int     size_t;
#endif
typedef int              ssize_t;
typedef long             suseconds_t;
typedef long             time_t;
typedef unsigned long    useconds_t;


#endif
