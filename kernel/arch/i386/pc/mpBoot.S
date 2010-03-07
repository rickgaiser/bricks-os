/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2010 Maximus32 <Maximus32@bricks-os.org>
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


#define KERNEL_CS 0x08
#define KERNEL_DS 0x10


  //.extern arch_main
  
  .global ap_boot_start
  .global ap_boot_end
  .global ap_stack_pointer
  .global ap_running


/*
 ******************************
 * Real-Mode, running as a copy
 ******************************
 */
  .text
  .align 4,0x0
  .code16
ap_boot_start:
  cli

  /* Data segment same as code segment */
  movw %cs, %ax
  movw %ax, %ds

  /* Load GDT */
  lgdtl ap_gdt32_desc - ap_boot_start

  /* Enable protected mode */
  movl %cr0, %eax
  orl $1, %eax
  movl %eax, %cr0

  /* Jump into protected mode */
  ljmpl $KERNEL_CS, $ap_boot_start_pm

ap_gdt32_table:
  .quad 0x0000000000000000 /* 0x00: Null descroptor */
  .quad 0x00cf9a000000ffff /* 0x08: Code segment */
  .quad 0x00cf92000000ffff /* 0x10: Data segment */
ap_gdt32_desc:
  .word ap_gdt32_desc - ap_gdt32_table - 1
  .long ap_gdt32_table

ap_boot_end:


/*
 ****************************
 * Protected-Mode, NOT copied
 ****************************
 */
  .code32
ap_boot_start_pm:
  /* Set initial segment selectors */
  movw $KERNEL_DS, %ax
  movw %ax, %ds
  movw %ax, %ss
  movw %ax, %es
  movw %ax, %fs
  movw %ax, %gs
  
  /* Let BSP know we're up and running */
  movb $1, ap_running
  
  /* Set stack pointer */
  movl ap_stack_pointer, %esp

  /* Jump to C routine */
  //call arch_main

  /* We should never get here! */
2:
  cli
  hlt
  jmp 2b

  .data
ap_stack_pointer:
  .long 0x00000000
ap_running:
  .byte 0x00

