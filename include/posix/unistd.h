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


#ifndef USER_UNISTD_H
#define USER_UNISTD_H


#include "sys/types.h"


#ifndef NULL
#define NULL (0)
#endif

#define STDIN   0
#define STDOUT  1
#define STDERR  2


#ifdef __cplusplus
extern "C" {
#endif


int     close(int fd);
ssize_t read (int fd, void * buffer, size_t size);
ssize_t write(int fd, const void * buffer, size_t size);


#ifdef __cplusplus
}
#endif


#endif
