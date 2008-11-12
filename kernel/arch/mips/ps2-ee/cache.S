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


/* Copyright (c) 2007 Mega Man */
/* Copyright (c) 2008 Maximus32 */
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
