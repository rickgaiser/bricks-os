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


#ifndef I386_DESCRIPTOR_H
#define I386_DESCRIPTOR_H


#include "asm/hal.h"


// -----------------------------------------------------------------------------
#define descriptorPresent(desc) (desc.access & DESC_PRESENT)


// -----------------------------------------------------------------------------
enum EDescriptorTableType
{
    dttInterrupt  // 1 table
  , dttGlobal     // 1 table
  , dttLocal      // 0..* tables
};

// -----------------------------------------------------------------------------
class CDescriptorTable
{
public:
  CDescriptorTable();
  ~CDescriptorTable();

  void init(SDescriptor * desc, unsigned int count, EDescriptorTableType type = dttLocal);

  // Segments (Create in first free entry, only for LDT/GDT)
  selector_t createSegment(EDescriptorType type, unsigned int privilege, uint32_t base, uint32_t limit);

  // Gates (Create in first free entry)
  selector_t createCallGate     (unsigned int privilege, selector_t selector, unsigned int params);
  selector_t createTaskGate     (unsigned int privilege, selector_t selector);
  selector_t createInterruptGate(unsigned int privilege, selector_t selector, uint32_t offset);
  selector_t createTrapGate     (unsigned int privilege, selector_t selector, uint32_t offset);

  // Gates (Create in specified entry)
  static void createCallGate     (SDescriptor & desc, unsigned int privilege, selector_t selector, unsigned int params);
  static void createTaskGate     (SDescriptor & desc, unsigned int privilege, selector_t selector);
  static void createInterruptGate(SDescriptor & desc, unsigned int privilege, selector_t selector, uint32_t offset);
  static void createTrapGate     (SDescriptor & desc, unsigned int privilege, selector_t selector, uint32_t offset);

  SDescriptorTableReg * getDTRPointer(); // Warning! This class will KEEP the ownership of the DTR

//private:
  SDescriptor         * desc_; // (Global/Local/Interrupt) Descriptor Table (GDT/LDT/IDT) (Max. Size = 8192 * 8 = 64KiB)
  SDescriptorTableReg   dtr_;  // (Global/Local/Interrupt) Descriptor Table Register (GDTR/LDTR/IDTR)
  EDescriptorTableType  eType_;
  unsigned int          iCount_;
};

// -----------------------------------------------------------------------------
void init_gdt(SDescriptor * desc, unsigned int count);
void init_idt(SDescriptor * desc, unsigned int count);


// -----------------------------------------------------------------------------
extern CDescriptorTable cGDT;
extern CDescriptorTable cIDT;

extern selector_t selNull;
extern selector_t selCodeKernel;
extern selector_t selDataKernel;
extern selector_t selCodeUser;
extern selector_t selDataUser;
extern selector_t selCodeUserTmp;
extern selector_t selDataUserTmp;


#endif
