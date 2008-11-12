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


#ifndef MIPSREGISTER_H
#define MIPSREGISTER_H


// MIPS CPU Registers
#define zero    $0  // Always 0
#define at      $1  // Assembler temporary
#define v0      $2  // Function return
#define v1      $3  //
#define a0      $4  // Function argument 0
#define a1      $5  // Function argument 1
#define a2      $6  // Function argument 2
#define a3      $7  // Function argument 3
#define t0      $8  // Temporaries. No need
#define t1      $9  // to preserve in your
#define t2      $10 // functions.
#define t3      $11
#define t4      $12
#define t5      $13
#define t6      $14
#define t7      $15
#define s0      $16 // Saved Temporaries.
#define s1      $17 // Make sure to restore
#define s2      $18 // to original value
#define s3      $19 // if your function
#define s4      $20 // changes their value.
#define s5      $21
#define s6      $22
#define s7      $23
#define t8      $24 // More Temporaries.
#define t9      $25
#define k0      $26 // Reserved for Kernel
#define k1      $27
#define gp      $28 // Global Pointer
#define sp      $29 // Stack Pointer
#define fp      $30 // Frame Pointer
#define ra      $31 // Function Return Address


#endif
