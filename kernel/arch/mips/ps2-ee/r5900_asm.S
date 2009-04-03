/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2007 Mega Man
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


#include "asm/mipsregisters.h"


/** Data Cache: Index Writeback Invalidate. */
#define DXWBIN 0x14

/** Instruction Cache: Index Invalidate. */
#define IXIN 0x07

/** Size of a cache line. */
#define CACHE_LINE_SIZE 64

/** Number of cache ways. */
#define NUMBER_OF_CACHE_WAYS 2

/** Number of data cache lines per cache way. */
#define NUMBER_OF_DCACHE_LINES 64

/** Number of instruction cache lines per cache way. */
#define NUMBER_OF_ICACHE_LINES 128

// Mapped address used for some tricks in exception handler,
// mapped at the end of the memory to get accesses to addresses
// calculated by (register zero - offset) working.
#define ZERO_REG_ADDRESS 0x78000

#define KSEG0 0x80000000

// Stack overhead, functions will touch into the area without moving stack pointer
#define STACK_OVERHEAD 16
// Align stack addresses at this value
#define STACK_ALIGN 16

// CPU registers
#define SAVE_REG(r)    sq $r, 16 * r(sp)
#define RESTORE_REG(r) lq $r, 16 * r(sp)
#define CPU_REG_SIZE (32 * 16)

// FPU registers
#define FPU_OFFSET CPU_REG_SIZE
#define SAVE_FPU(r)    swc1 $r, FPU_OFFSET + 8 * r(sp)
#define RESTORE_FPU(r) lwc1 $r, FPU_OFFSET + 8 * r(sp)
#define FPU_REG_SIZE (32 * 8)

// Status registers
#define STATUS_OFFSET (FPU_OFFSET + FPU_REG_SIZE)
#define STATUS_REG_SIZE (9 * 4)

// Size of the context (aligned to STACK_ALIGN)
#define CONTEXT_SIZE (((CPU_REG_SIZE + FPU_REG_SIZE + STATUS_REG_SIZE) + STACK_ALIGN - 1) & ~(STACK_ALIGN - 1))


.data
exceptionKernelStack:
.long KSEG0 + ZERO_REG_ADDRESS - STACK_OVERHEAD


.text

.globl flushDCacheAll
.type flushDCacheAll,@function;
.ent flushDCacheAll
.set noreorder
/* Cache instructions as last instuction of a page could lead to errors. */
.align 6
flushDCacheAll:
	li	t0, 0x00000000
	li	t1, NUMBER_OF_DCACHE_LINES
flushDCacheAll_loop:
	sync.l
	/* Flush cache line in Way 0. */
	cache DXWBIN, 0(t0)
	sync.l
	/* Flush cache line in Way 1. */
	cache DXWBIN, 1(t0)
	sync.l
	addi	t1, t1, -1
	addi	t0, t0, CACHE_LINE_SIZE
	bgtz	t1, flushDCacheAll_loop
	nop
	jr	ra
	nop
.end flushDCacheAll

.globl invalidateICacheAll
.type invalidateICacheAll,@function;
.ent invalidateICacheAll
.set noreorder
/* Align at cache line size to be sure, that only one cache line is loaded by code itself. */
.align 6
invalidateICacheAll:
	li	t0, 0x00000000
	li	t1, NUMBER_OF_ICACHE_LINES
invalidateICacheAll_loop:
	sync.p
	/* Flush cache line in Way 0. */
	cache IXIN, 0(t0)
	sync.p
	/* Flush cache line in Way 1. */
	cache IXIN, 1(t0)
	sync.p
	addi	t1, t1, -1
	addi	t0, t0, CACHE_LINE_SIZE
	bgtz	t1, invalidateICacheAll_loop
	nop
	jr	ra
	nop
.end invalidateICacheAll

.globl commonExceptionHandler
.type commonExceptionHandler,@function;
.ent commonExceptionHandler
.set noat
.set noreorder
commonExceptionHandler:
        // Get exception cause
        mfc0    k0, $13
        sync.p
        srl     k0, k0, 2
        andi    k0, k0, 0x1f
        li      k1, 0x8
        beq     k0, k1, callSyscallHandler
        nop

        addiu   k0, sp, 0
        lw      sp, exceptionKernelStack
        addiu   sp, sp, -STACK_OVERHEAD
        addiu   sp, sp, -CONTEXT_SIZE

        // Save 32 128-bit General Purpose Registers (GPRs)
        SAVE_REG(0)
        SAVE_REG(1)
        SAVE_REG(2)
        SAVE_REG(3)
        SAVE_REG(4)
        SAVE_REG(5)
        SAVE_REG(6)
        SAVE_REG(7)
        SAVE_REG(8)
        SAVE_REG(9)
        SAVE_REG(10)
        SAVE_REG(11)
        SAVE_REG(12)
        SAVE_REG(13)
        SAVE_REG(14)
        SAVE_REG(15)
        SAVE_REG(16)
        SAVE_REG(17)
        SAVE_REG(18)
        SAVE_REG(19)
        SAVE_REG(20)
        SAVE_REG(21)
        SAVE_REG(22)
        SAVE_REG(23)
        SAVE_REG(24)
        SAVE_REG(25)
        SAVE_REG(26)
        SAVE_REG(27)
        SAVE_REG(28)
        sq k0, 16 * 29(sp)
        SAVE_REG(30)
        SAVE_REG(31)

        // Param 1: Pointer to CPU context
        addiu   a0, sp, 0
        jal     errorHandler
        nop
        eret

callSyscallHandler:
        // for debugging kernel exceptions
#if 0
        mfc0    k0, $12
        sync.p
        // Switch to kernel mode and disable EXL
        ori     k0, k0, 0x1a
        xori    k0, k0, 0x1a
        mtc0    k0, $12
        sync.p
#endif
        bltzl   v1, negative_syscall
        // Make v1 positive
        negu    v1, v1
negative_syscall:

        addiu   sp, -0x10
        sw      ra, 0(sp)
        // Save EPC
        mfc0    k0, $14
        sync.p
        addiu   k0, 4
        sw      k0, 4(sp)
        mtc0    k0, $14
        sync.p

        // Check for last syscall number
        sltiu   k0, v1, 130
        blez    k0, undefinedSyscall
        nop

        // Make lookup in syscallTable
        la      k0, syscallTable
        sll     k1, v1, 2
        addu    k0, k0, k1
        lw      k0, 0(k0)

        // If value is zero, Syscall is not implemented
        beq     k0, zero, undefinedSyscall
        nop

        // Call syscall function
        jalr    k0
        nop

        j       commonExceptionHandlerExit
        nop

undefinedSyscall:
        move    a0, v1
        jal     unknownSyscall
        nop

commonExceptionHandlerExit:
        lw      k0, 4(sp)
        lw      ra, 0(sp)
        addiu   sp, 0x10
        mtc0    k0, $14
        sync.p
        sync.l
        eret
.end commonExceptionHandler


// Exception handler for MIPS_EXCEPTION_INTERRUPT
.globl  interruptExceptionHandler
.type interruptExceptionHandler,@function;
.ent interruptExceptionHandler
.set noat
.set noreorder
interruptExceptionHandler:
        // Save stack pointer to k0
        addiu   k0, sp, 0

        // Use different stack
        lw      sp, exceptionKernelStack

reuseStack:
        addiu   sp, sp, -STACK_OVERHEAD
        addiu   sp, sp, -CONTEXT_SIZE

        // Save 32 128-bit General Purpose Registers (GPRs)
        SAVE_REG(0)
        SAVE_REG(1)
        SAVE_REG(2)
        SAVE_REG(3)
        SAVE_REG(4)
        SAVE_REG(5)
        SAVE_REG(6)
        SAVE_REG(7)
        SAVE_REG(8)
        SAVE_REG(9)
        SAVE_REG(10)
        SAVE_REG(11)
        SAVE_REG(12)
        SAVE_REG(13)
        SAVE_REG(14)
        SAVE_REG(15)
        SAVE_REG(16)
        SAVE_REG(17)
        SAVE_REG(18)
        SAVE_REG(19)
        SAVE_REG(20)
        SAVE_REG(21)
        SAVE_REG(22)
        SAVE_REG(23)
        SAVE_REG(24)
        SAVE_REG(25)
        SAVE_REG(26)
        SAVE_REG(27)
        SAVE_REG(28)
        sq k0, 16 * 29(sp)
        SAVE_REG(30)
        SAVE_REG(31)

        // Save 32-bit COP0-Reg14, Exception Program Counter (EPC)
        mfc0    t5, $14
        sync.p
        sw      t5, STATUS_OFFSET + 4 * 8(sp)

        // Save 32-bit CP1-FCR31, FPU Control/Status register
        cfc1    t6, $31
        sw      t6, STATUS_OFFSET + 4 * 7(sp)

        // Save 32 64-bit Floating Point registers (FPRs) to stack
        SAVE_FPU(0)
        SAVE_FPU(1)
        SAVE_FPU(2)
        SAVE_FPU(3)
        SAVE_FPU(4)
        SAVE_FPU(5)
        SAVE_FPU(6)
        SAVE_FPU(7)
        SAVE_FPU(8)
        SAVE_FPU(9)
        SAVE_FPU(10)
        SAVE_FPU(11)
        SAVE_FPU(12)
        SAVE_FPU(13)
        SAVE_FPU(14)
        SAVE_FPU(15)
        SAVE_FPU(16)
        SAVE_FPU(17)
        SAVE_FPU(18)
        SAVE_FPU(19)
        SAVE_FPU(20)
        SAVE_FPU(21)
        SAVE_FPU(22)
        SAVE_FPU(23)
        SAVE_FPU(24)
        SAVE_FPU(25)
        SAVE_FPU(26)
        SAVE_FPU(27)
        SAVE_FPU(28)
        SAVE_FPU(29)
        SAVE_FPU(30)
        SAVE_FPU(31)

        // Save fp_acc
        mtc1    $0, $0
        msub.s  $f0, $f0, $f0   // $f0 = fpacc - 0.0f * 0.0f
        swc1    $0, STATUS_OFFSET + 4 * 0(sp)

        // Save HI and LO Registers (4*64-bit)
        mfhi    t0
        sw      t0, STATUS_OFFSET + 4 * 2(sp)
        mflo    t1
        sw      t1, STATUS_OFFSET + 4 * 3(sp)
        mfhi1   t2
        sw      t2, STATUS_OFFSET + 4 * 4(sp)
        mflo1   t3
        sw      t3, STATUS_OFFSET + 4 * 5(sp)
        mfsa    t4
        sw      t4, STATUS_OFFSET + 4 * 6(sp)

        // Parameter 2: Register context
        move a0, sp
        jal isr
        nop

        // Restore HI and LO Registers (4*64-bit)
        lw      t0, STATUS_OFFSET + 4 * 2(sp)
        mthi    t0
        lw      t1, STATUS_OFFSET + 4 * 3(sp)
        mtlo    t1
        lw      t2, STATUS_OFFSET + 4 * 4(sp)
        mthi1   t2
        lw      t3, STATUS_OFFSET + 4 * 5(sp)
        mtlo1   t3
        lw      t4, STATUS_OFFSET + 4 * 6(sp)
        mtsa    t4

        // Restore fp_acc
        lwc1    $0, STATUS_OFFSET + 4 * 0(sp)
        mtc1    $0, $1
        suba.s  $f0, $f1  // fpacc <- $f0 - 0.0f

        // Restore 32 64-bit Floating Point registers (FPRs) to stack
        RESTORE_FPU(0)
        RESTORE_FPU(1)
        RESTORE_FPU(2)
        RESTORE_FPU(3)
        RESTORE_FPU(4)
        RESTORE_FPU(5)
        RESTORE_FPU(6)
        RESTORE_FPU(7)
        RESTORE_FPU(8)
        RESTORE_FPU(9)
        RESTORE_FPU(10)
        RESTORE_FPU(11)
        RESTORE_FPU(12)
        RESTORE_FPU(13)
        RESTORE_FPU(14)
        RESTORE_FPU(15)
        RESTORE_FPU(16)
        RESTORE_FPU(17)
        RESTORE_FPU(18)
        RESTORE_FPU(19)
        RESTORE_FPU(20)
        RESTORE_FPU(21)
        RESTORE_FPU(22)
        RESTORE_FPU(23)
        RESTORE_FPU(24)
        RESTORE_FPU(25)
        RESTORE_FPU(26)
        RESTORE_FPU(27)
        RESTORE_FPU(28)
        RESTORE_FPU(29)
        RESTORE_FPU(30)
        RESTORE_FPU(31)

        // Restore 32-bit CP1-FCR31, FPU Control/Status register
        lw      t5, STATUS_OFFSET + 4 * 7(sp)
        ctc1    t5, $31

        // Restore 32-bit COP0-Reg14, Exception Program Counter (EPC)
        lw      t6, STATUS_OFFSET + 4 * 8(sp)
        mtc0    t6, $14
        sync.p

        // Restore 32 128-bit General Purpose Registers (GPRs)
        RESTORE_REG(0)
        RESTORE_REG(1)
        RESTORE_REG(2)
        RESTORE_REG(3)
        RESTORE_REG(4)
        RESTORE_REG(5)
        RESTORE_REG(6)
        RESTORE_REG(7)
        RESTORE_REG(8)
        RESTORE_REG(9)
        RESTORE_REG(10)
        RESTORE_REG(11)
        RESTORE_REG(12)
        RESTORE_REG(13)
        RESTORE_REG(14)
        RESTORE_REG(15)
        RESTORE_REG(16)
        RESTORE_REG(17)
        RESTORE_REG(18)
        RESTORE_REG(19)
        RESTORE_REG(20)
        RESTORE_REG(21)
        RESTORE_REG(22)
        RESTORE_REG(23)
        RESTORE_REG(24)
        RESTORE_REG(25)
        RESTORE_REG(26)
        RESTORE_REG(27)
        RESTORE_REG(28)
        RESTORE_REG(30)
        RESTORE_REG(31)
        RESTORE_REG(29)

        sync.l
        sync.p
        eret
.end interruptExceptionHandler

.global jumpToSegment
.type jump2kernelspace,@function;
.ent jumpToSegment
jumpToSegment:
        or ra, ra, a0
        or sp, sp, a0
        jr ra
        nop
.end jumpToSegment
