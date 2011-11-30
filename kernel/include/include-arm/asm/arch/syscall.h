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


#ifndef ASM_ARCH_SYSCALL_H
#define ASM_ARCH_SYSCALL_H


#include "stdint.h"


// Function pointer definitions
//typedef void (* fnptr        )();
typedef int  (* pf_msgSend   )(int, const void *, int, void *, int);
typedef int  (* pf_msgReceive)(int, void *, int);
typedef int  (* pf_msgReply  )(int, int, const void *, int);


#ifdef GBA
#define IWRAM_BASE            (0x03000000)
#define IWRAM_TOP             (IWRAM_BASE + 0x8000)

//#define REG_INTMAIN           (*(fnptr         *)(IWRAM_TOP -  4))
#define REG_MSGSEND           (*(pf_msgSend    *)(IWRAM_TOP -  8))
#define REG_MSGRECEIVE        (*(pf_msgReceive *)(IWRAM_TOP - 12))
#define REG_MSGREPLY          (*(pf_msgReply   *)(IWRAM_TOP - 16))
#endif


#endif
