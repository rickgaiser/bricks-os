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


#include "mp.h"
#include "apic.h"
#include "stddef.h"
#include "string.h"
#include "kernel/debug.h"


#define BDA_EBDA_ADDR     (*(vuint16_t *)0x0000040e)
#define AP_ENTRY_PAGE     (6)
#define AP_ENTRY_ADDR     (AP_ENTRY_PAGE<<12)

extern uint8_t ap_boot_start;
extern uint8_t ap_boot_end;
extern uint32_t ap_stack_pointer;
extern vuint8_t ap_running;


//---------------------------------------------------------------------------
bool
mpHeaderValidate(SMPTableHeader * pHeader)
{
  uint32_t iLength = pHeader->iBaseTableLength;
  uint8_t * pBase = (uint8_t *)pHeader;
  
  uint8_t iChecksum = 0;
  for(uint32_t i(0); i < iLength; i++)
    iChecksum += *pBase++;
  
  return iChecksum == 0;
}

//---------------------------------------------------------------------------
bool
mpPointerValidate(SMPFloatingPointer * pPointer)
{
  uint32_t iLength = pPointer->iLength * 16;
  uint8_t * pBase = (uint8_t *)pPointer;
  
  uint8_t iChecksum = 0;
  for(uint32_t i(0); i < iLength; i++)
    iChecksum += *pBase++;
  
  return iChecksum == 0;
}

//---------------------------------------------------------------------------
// Search for "MP Floating Pointer Structure"
SMPFloatingPointer * 
mpPointerLocateInRegion(uint8_t * base, uint32_t length)
{
  for(uint32_t i(0); i < length; i += 16)
  {
    uint32_t * pSignature = (uint32_t *)(base + i);

    if((*pSignature == MP_FLOATING_SIGNATURE) && (mpPointerValidate((SMPFloatingPointer *)pSignature) == true))
      return (SMPFloatingPointer *)pSignature;
  }
  
  return NULL;
}

//---------------------------------------------------------------------------
void
init_mp()
{
  unsigned int iApicIds[16];
  unsigned int iCPUCount = 0;
  
  // Try to locate the "MP Floating Pointer Structure"
  SMPFloatingPointer * pMPPointer = NULL;
  if(BDA_EBDA_ADDR != 0)
  {
    //  - 1: In the first KiB of the "Extended BIOS Data Area" / EBDA
    pMPPointer = mpPointerLocateInRegion((uint8_t *)(BDA_EBDA_ADDR << 4), 1*1024);
  }
  else
  {
    //  - 2: In the last KiB of base memory (640KiB)
    pMPPointer = mpPointerLocateInRegion((uint8_t *)((640 - 1) * 1024), 1*1024);
    //  - 2: In the last KiB of base memory (512KiB)
    if(pMPPointer == NULL)
      pMPPointer = mpPointerLocateInRegion((uint8_t *)((512 - 1) * 1024), 1*1024);
  }
  if(pMPPointer == NULL)
  {
    //  - 3: In the BIOS ROM (0xf0000-0xfffff, 64KiB)
    pMPPointer = mpPointerLocateInRegion((uint8_t *)(0xf0000), 64*1024);
  }
  if(pMPPointer != NULL)
  {
    // NOTE: Is this the correct way to interpret the revision?
    printk("found: MP Floating Pointer Structure (v1.%d)\n", pMPPointer->iSpecRev);
    
    SMPTableHeader * pMPHeader = (SMPTableHeader *)pMPPointer->pAddr;
    if((pMPHeader != NULL) && (mpHeaderValidate(pMPHeader) == true))
    {
      printk("found: MP Configuration Table (v1.%d)\n", pMPHeader->iSpecRev);
      
      //printk(" - ");
      //for(int i(0); i <  8; i++) printk("%c", pMPHeader->sOemId[i]);
      //for(int i(0); i < 12; i++) printk("%c", pMPHeader->sProdId[i]);
      //printk("\n");
      
      uint8_t * pEntry = (uint8_t *)(pMPPointer->pAddr + sizeof(SMPTableHeader));
      for(int i(0); i < pMPHeader->iEntryCount; i++)
      {
        switch(*pEntry)
        {
          case 0:
          {
            SMPTableEntryProcessor * pProcessor = (SMPTableEntryProcessor *)pEntry;
            printk(" - Processor (id=%d)\n", pProcessor->iLocalApicId);

            iApicIds[iCPUCount] = pProcessor->iLocalApicId;
            iCPUCount++;
            
            pEntry += 20;
            break;
          }
          case 1:
            printk(" - Bus: ");
            for(int j(0); j < 6; j++) printk("%c", pEntry[2+j]);
            printk("\n");
            pEntry += 8;
            break;
          case 2:
            printk(" - I/O Apic\n");
            pEntry += 8;
            break;
          case 3:
            //printk(" - I/O Interrupt Assignment\n");
            pEntry += 8;
            break;
          case 4:
            //printk(" - Local Interrupt Assignment\n");
            pEntry += 8;
            break;
          default:
            printk(" - Unknown\n");
        };
      }
    }
    else
    {
      printk("Warning: MP Configuration Table not found\n");
    }
  }
  
  if(iCPUCount > 1)
  {
    printk("AP entry point located at 0x%x, size=%d\n", &ap_boot_start, &ap_boot_end - &ap_boot_start);
    
    // Copy Real-Mode code to AP boot location
    memcpy((void *)AP_ENTRY_ADDR, &ap_boot_start, &ap_boot_end - &ap_boot_start);
    // Set initial stack for AP
    ap_stack_pointer = 0x00108000;
    // Clear running flag
    ap_running = 0;
    
    // Try to wake up the AP with an INIT IPI
    REG_APIC_ICR_HIGH = iApicIds[1] << 24;
    REG_APIC_ICR_LOW = 5<<8;
    // Wait for delivery
    while(REG_APIC_ICR_LOW & (1<<12));
    // Wait 10ms (10000us)
    for(volatile unsigned int i = 0; i < 10000; i++)
    {
      for(volatile unsigned int j = 0; j < 2000; j++);
      if(ap_running == 1)
        break;
    }
    
    if(ap_running == 0)
    {
      // Try to wake up the AP with a STARTUP IPI
      REG_APIC_ICR_HIGH = iApicIds[1] << 24;
      REG_APIC_ICR_LOW = 6<<8 | AP_ENTRY_PAGE;
      // Wait for delivery
      while(REG_APIC_ICR_LOW & (1<<12));
      // Wait 200us
      for(volatile unsigned int i = 0; i < 200; i++)
      {
        for(volatile unsigned int j = 0; j < 2000; j++);
        if(ap_running == 1)
          break;
      }

      if(ap_running == 0)
      {    
      // Try to wake up the AP with a second STARTUP IPI
        REG_APIC_ICR_HIGH = iApicIds[1] << 24;
        REG_APIC_ICR_LOW = 6<<8 | 7;
        // Wait for delivery
        while(REG_APIC_ICR_LOW & (1<<12));
        // Wait 200us
        for(volatile unsigned int i = 0; i < 200; i++)
        {
          for(volatile unsigned int j = 0; j < 2000; j++);
          if(ap_running == 1)
            break;
        }
      }
    }
    
    if(ap_running == 1)
    {
      printk("AP Running!!!\n");
    }

    while(1);
  }
}


