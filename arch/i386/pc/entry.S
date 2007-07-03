/* boot.S - bootstrap the kernel */
/* Copyright (C) 1999, 2001  Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#define ASM	1
#include <multiboot.h>
	
	.text

	.globl entry, multiboot_header
entry:
	jmp multiboot_entry

	// Align 32 bits boundary.
	.align 4
multiboot_header:
	.long MULTIBOOT_HEADER_MAGIC // magic
	.long MULTIBOOT_HEADER_FLAGS // flags
	.long -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS) // checksum
#ifndef __ELF__
	.long multiboot_header // header_addr
	.long _start           // load_addr
	.long _edata           // load_end_addr
	.long _end             // bss_end_addr
	.long multiboot_entry  // entry_addr
#endif // __ELF__

multiboot_entry:
	// Set stack pointer
	movl $0x00101000, %esp
	// call __main(unsigned long magic, multiboot_info_t * mbi)
	pushl %ebx
	pushl %eax
	call EXT_C(__main)

