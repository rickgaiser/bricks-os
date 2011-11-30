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


#ifndef UNISTD_K_H
#define UNISTD_K_H


#include "kernel/syscall.h"
#include "sys/types.h"
#include "stdint.h"


#ifdef __cplusplus
extern "C" {
#endif


//declareSysCallKernel1r(int,             brk, void *, addr);
//declareSysCallKernel1 (                 _exit, int, iStatus);
declareSysCallKernel0r(pid_t,           getpid);
//declareSysCallKernel1r(char *,          sbrk, intptr_t, increment);
declareSysCallKernel1r(unsigned int,    sleep, unsigned int, iSeconds);
declareSysCallKernel1r(int,             usleep, useconds_t, useconds);


#ifdef __cplusplus
}
#endif


#endif
