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


#include "kernel/srr.h"
#include "kernel/syscall.h"
#include "pthread.h"


//---------------------------------------------------------------------------
// Channels
//---------------------------------------------------------------------------
sysCallUser1r(int, channelCreate,          unsigned, iFlags);
sysCallUser1r(int, channelDestroy,         int, iChannelID);
sysCallUser4r(int, channelConnectAttach,   uint32_t, iNodeID, pid_t, iProcessID, int, iChannelID, int, iFlags);
sysCallUser1r(int, channelConnectDetach,   int, iConnectionID);

//---------------------------------------------------------------------------
// Names
//---------------------------------------------------------------------------
sysCallUser2r(int, registerName,           int, channelID, const char *, name);
sysCallUser3r(int, lookupName,             const char *, name, int *, pid, int *, channelID);

//---------------------------------------------------------------------------
// Threads
//---------------------------------------------------------------------------
sysCallUser4r(int, pthread_create,         pthread_t *, thread, const pthread_attr_t *, attr, pthread_func_entry, start_routine, void *, arg);
sysCallUser1 (     pthread_exit,           void *, status);
sysCallUser2r(int, pthread_join,           pthread_t, thread, void **, value_ptr);

//---------------------------------------------------------------------------
// Condition
//---------------------------------------------------------------------------
sysCallUser2r(int, pthread_cond_init,      pthread_cond_t *, cond, const pthread_condattr_t *, attr);
sysCallUser1r(int, pthread_cond_destroy,   pthread_cond_t *, cond);
sysCallUser2r(int, pthread_cond_wait,      pthread_cond_t *, cond, pthread_mutex_t *, mutex);
sysCallUser3r(int, pthread_cond_timedwait, pthread_cond_t *, cond, pthread_mutex_t *, mutex, const struct timespec *, ts);
sysCallUser1r(int, pthread_cond_signal,    pthread_cond_t *, cond);
sysCallUser1r(int, pthread_cond_broadcast, pthread_cond_t *, cond);

//---------------------------------------------------------------------------
// Mutex
//---------------------------------------------------------------------------
sysCallUser2r(int, pthread_mutex_init,     pthread_mutex_t *, mutex, const pthread_mutexattr_t *, attr);
sysCallUser1r(int, pthread_mutex_destroy,  pthread_mutex_t *, mutex);
sysCallUser1r(int, pthread_mutex_lock,     pthread_mutex_t *, mutex);
sysCallUser1r(int, pthread_mutex_trylock,  pthread_mutex_t *, mutex);
sysCallUser1r(int, pthread_mutex_unlock,   pthread_mutex_t *, mutex);

//---------------------------------------------------------------------------
// Other
//---------------------------------------------------------------------------
//sysCallUser1r(int,             brk, void *, addr);
//sysCallUser1 (                 _exit, int, iStatus);
sysCallUser0r(pid_t,           getpid);
//sysCallUser1r(char *,          sbrk, intptr_t, increment);
sysCallUser1r(unsigned int,    sleep, unsigned int, iSeconds);
sysCallUser1r(int,             usleep, useconds_t, useconds);
