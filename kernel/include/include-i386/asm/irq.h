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


#ifndef ASM_IRQ_H
#define ASM_IRQ_H


#define MAX_INTERRUPTS 0x31


// -----------------------------------------------------------------------------
struct pt_regs
{
  // pushed by pusha
  unsigned long edi, esi, ebp, esp, ebx, edx, ecx, eax;
  // pushed separately
  unsigned long ss, ds, es, fs, gs;
  unsigned long iIntNumber, iErrorCode;
  // pushed by exception. Exception may also push err_code.
  // user_esp and user_ss are pushed only if a privilege change occurs.
  unsigned long eip, cs, eflags, user_esp, user_ss;
};


#endif
