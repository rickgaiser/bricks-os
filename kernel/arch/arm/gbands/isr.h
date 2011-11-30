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


#ifndef GBANDS_ISR_H
#define GBANDS_ISR_H


#include "stdint.h"
#include "multiBoot.h"
#include "asm/irq.h"


#ifdef __cplusplus
extern "C"
{
#endif


// MultiBoot
uint32_t MultiBoot(MultiBootParam * mp, uint32_t mode);

// Local interrupts, defined in "asm/cpu.h"
//unsigned long local_save_flags(void);
//void local_irq_restore(unsigned long flags);
//void local_irq_disable(void);
//void local_irq_enable(void);

// Task switch
void __jump(pt_regs * current_thread, pt_regs * new_thread);

// ISR
void __gba_isr();
void __isr();


#ifdef __cplusplus
};
#endif


#endif
