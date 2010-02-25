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


#define ASM	1
#include "multiboot.h"
#include "pmm.h"
	

// FIXME: This is not in the spec
#define GRUB_KERNEL_CS  0x08
#define GRUB_KERNEL_DS  0x10

#define PG_PRESENT      (1<<0)
#define PG_WRITABLE     (1<<1)
#define PG_USER         (1<<2)
#define PG_GLOBAL       (1<<8)
#define PG_KERNEL_FLAGS (PG_GLOBAL | PG_WRITABLE | PG_PRESENT)

#define VIRT_TO_PHYS(a) ((a) - KERNEL_VIRTUAL_OFFSET)
#define PHYS_TO_VIRT(a) ((a) + KERNEL_VIRTUAL_OFFSET)
#define kernel_page_table(x) (kernel_page_tables + ((x - 992) * PAGE_SIZE))


	.extern __main
	
	.global start
	.global multiboot_header


	.section .text
start:
	jmp start2

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
	.long start2           // entry_addr
#endif // __ELF__


// Running at physical memory location
start2:
	// Set stack pointer, and save multiboot values on the stack
	movl $VIRT_TO_PHYS(initial_stack_top), %esp
	pushl %ebx
	pushl %eax

  // Create page tables
  //  -    0: The lower 4MiB is identity mapped
  movl $0, %eax
  movl $0x00000000, %ebx
fill_table_0:
  movl %ebx, %ecx
  orl $PG_KERNEL_FLAGS, %ecx
  movl %ecx, VIRT_TO_PHYS(page_table_0)(,%eax,4)
  addl $PAGE_SIZE, %ebx
  incl %eax
  cmpl $PAGES_IN_PT, %eax
  jne fill_table_0
  //  -  992: The kernel at 4GiB-128MiB
  movl $0, %eax
  movl $PHYS_KERNEL_BASE, %ebx
fill_table_992:
  movl %ebx, %ecx
  orl $PG_KERNEL_FLAGS, %ecx
  movl %ecx, VIRT_TO_PHYS(kernel_page_table(992))(,%eax,4)
  addl $PAGE_SIZE, %ebx
  incl %eax
  cmpl $PAGES_IN_PT, %eax
  jne fill_table_992
  //  - 1021: The kernel stack is mapped to 4GiB - 12MiB
  movl $(PAGES_IN_PT-PHYS_STACK_PAGE_COUNT), %eax
  movl $initial_stack, %ebx
fill_table_1021:
  movl %ebx, %ecx
  orl $PG_KERNEL_FLAGS, %ecx
  movl %ecx, VIRT_TO_PHYS(kernel_page_table(1021))(,%eax,4)
  addl $PAGE_SIZE, %ebx
  incl %eax
  cmpl $PAGES_IN_PT, %eax
  jne fill_table_1021

  // Create page directory
  //  - First 4MiB identity mapped
  movl $VIRT_TO_PHYS(page_table_0), %ecx
  orl $PG_KERNEL_FLAGS, %ecx
  movl %ecx, VIRT_TO_PHYS(initial_page_directory)+(0*4)
  //  - The entire kernel space
  movl $992, %eax
  movl $VIRT_TO_PHYS(kernel_page_table(992)), %ebx
fill_kernel_pd:
  movl %ebx, %ecx
  orl $PG_KERNEL_FLAGS, %ecx
  movl %ecx, VIRT_TO_PHYS(initial_page_directory)(,%eax,4)
  addl $PAGE_SIZE, %ebx
  incl %eax
  cmpl $(PAGES_IN_PT - 2), %eax
  jne fill_kernel_pd
  //  - Map pd into itself
  movl $VIRT_TO_PHYS(initial_page_directory), %ecx
  orl $PG_KERNEL_FLAGS, %ecx
  movl %ecx, VIRT_TO_PHYS(initial_page_directory)+(1023*4)
  
  // Set page directory
  movl $VIRT_TO_PHYS(initial_page_directory), %ecx
  movl %ecx, %cr3
  
  // Enable paging
  movl %cr0, %eax
  orl $0x80000000, %eax
  movl %eax, %cr0
  
  // Jump to kernel space
  ljmpl $GRUB_KERNEL_CS, $eip_in_kernel_space
eip_in_kernel_space:

	// Relocate stack pointer, and
	// call __main(unsigned long magic, multiboot_info_t * mbi)
	movl $(initial_stack_top - 8), %esp
	call __main
  // We should never get here
  cli
  hlt


  .section .bss
  .align 4096
  
  // Page Directory
  .global initial_page_directory
  .lcomm initial_page_directory, PAGE_SIZE

  // Page Tables
  // Allocate enough space for ALL kernel page tables (32bit table entries)
  // NOTE that the last 2 tables are mapped page directories, so we don't allocate page tables for them
  // Size: 120KiB (for a 128MiB kernel region)
  .lcomm kernel_page_tables, ((MM_KERNEL_PT_COUNT - 2) * PAGE_SIZE)

  // The first 4MiB is identity mapped
  // FIXME: This memory could be freed after initialization
  .lcomm page_table_0, PAGE_SIZE
  // Initial stack
  // FIXME: This memory could be freed after initialization
  .lcomm initial_stack, PHYS_STACK_SIZE
initial_stack_top:

