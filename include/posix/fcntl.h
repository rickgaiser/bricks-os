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


#ifndef FCNTL_H
#define FCNTL_H


#define O_RDONLY    0x0001 // open for reading only
#define O_WRONLY    0x0002 // open for writing only
#define O_RDWR      0x0000 // open for reading and writing
#define O_ACCMODE   0x0003 // mask for above modes

#define O_NONBLOCK  0x0004 // no delay
#define O_APPEND    0x0008 // set append mode
#define O_CREAT     0x0200 // create if nonexistant
#define O_TRUNC     0x0400 // truncate to zero length
#define O_EXCL      0x0800 // error if already exists


#ifdef __cplusplus
extern "C" {
#endif


//int creat(const char * path, mode_t mode);
//int fcntl(int fd, int cmd, ...);
int open(const char * path, int oflag, ...);


#ifdef __cplusplus
}
#endif


#endif
