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


// -----------------------------------------------------------------------------
// ISR Wrapper for interrupts WITHOUT an error code
#define isr_wrapper(nr) \
	.global isr_ ## nr ## _wrapper; \
isr_ ## nr ## _wrapper: \
	pushl $0; \
	pushl $nr; \
	jmp all_ints;

// -----------------------------------------------------------------------------
// ISR Wrapper for interrupts WITH an error code
#define isr_error_wrapper(nr) \
	.global isr_ ## nr ## _wrapper; \
isr_ ## nr ## _wrapper: \
	pushl $nr; \
	jmp all_ints; \


	.text
// -----------------------------------------------------------------------------
// Wrappers for all used interrupts
// -----------------------------------------------------------------------------
// Processor generated interrupts
isr_wrapper(0x00);
isr_wrapper(0x01);
isr_wrapper(0x02);
isr_wrapper(0x03);
isr_wrapper(0x04);
isr_wrapper(0x05);
isr_wrapper(0x06);
isr_wrapper(0x07);
isr_error_wrapper(0x08);
isr_wrapper(0x09);
isr_error_wrapper(0x0a);
isr_error_wrapper(0x0b);
isr_error_wrapper(0x0c);
isr_error_wrapper(0x0d);
isr_error_wrapper(0x0e);
isr_wrapper(0x0f);
isr_wrapper(0x10);
isr_wrapper(0x11);
isr_wrapper(0x12);
isr_wrapper(0x13);
isr_wrapper(0x14);
isr_wrapper(0x15);
isr_wrapper(0x16);
isr_wrapper(0x17);
isr_wrapper(0x18);
isr_wrapper(0x19);
isr_wrapper(0x1a);
isr_wrapper(0x1b);
isr_wrapper(0x1c);
isr_wrapper(0x1d);
isr_wrapper(0x1e);
isr_wrapper(0x1f);
// PIC generated interrupts
isr_wrapper(0x20);
isr_wrapper(0x21);
isr_wrapper(0x22);
isr_wrapper(0x23);
isr_wrapper(0x24);
isr_wrapper(0x25);
isr_wrapper(0x26);
isr_wrapper(0x27);
isr_wrapper(0x28);
isr_wrapper(0x29);
isr_wrapper(0x2a);
isr_wrapper(0x2b);
isr_wrapper(0x2c);
isr_wrapper(0x2d);
isr_wrapper(0x2e);
isr_wrapper(0x2f);
// Software generated interrupts
isr_wrapper(0x30);
isr_wrapper(0x80);

// -----------------------------------------------------------------------------
// Common part of interrupts
all_ints:
	// Save all registers
	pushl %gs;
	pushl %fs;
	pushl %es;
	pushl %ds;
	pushl %ss;
	pusha;
		// Set correct segment selectors
		//movl $(__USER_DS), %edx;
		movl $0x10, %edx;
		movl %edx, %ds;
		movl %edx, %es;
		movl %edx, %fs;
		movl %edx, %gs;
		// Call Interrupt Service Routine
		movl %esp, %eax;
		pushl %eax;
			call isr;
		popl %eax;
	// Restore all registers
	popa;
	popl %ss;
	popl %ds;
	popl %es;
	popl %fs;
	popl %gs;
	// Drop interrupt number and error code
	addl $8, %esp;
	// Return from interrupt
	iret;

