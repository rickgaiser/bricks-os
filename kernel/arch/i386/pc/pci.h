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
// Mostly from libpci from the Minix source code


#ifndef PC_PCI_H
#define PC_PCI_H


#include "stdint.h"


#define PCI_VID         0x00    /* Vendor ID, 16-bit */
#define PCI_DID         0x02    /* Device ID, 16-bit */
#define PCI_CR          0x04    /* Command Register, 16-bit */
#define PCI_PCISTS      0x06    /* PCI status, 16-bit */
#define PSR_SSE         0x4000  /* Signaled System Error */
#define PSR_RMAS        0x2000  /* Received Master Abort Status */
#define PSR_RTAS        0x1000  /* Received Target Abort Status */
#define PCI_REV         0x08    /* Revision ID */
#define PCI_PIFR        0x09    /* Prog. Interface Register */
#define PCI_SCR         0x0A    /* Sub-Class Register */
#define PCI_BCR         0x0B    /* Base-Class Register */
#define PCI_HEADT       0x0E    /* Header type, 8-bit */
#define PHT_MULTIFUNC   0x80    /* Multiple functions */
#define PCI_BAR         0x10    /* Base Address Register */
#define PCI_BAR_2       0x14    /* Base Address Register */
#define PCI_BAR_3       0x18    /* Base Address Register */
#define PCI_BAR_4       0x1C    /* Base Address Register */
#define PCI_ILR         0x3C    /* Interrupt Line Register */
#define PCI_IPR         0x3D    /* Interrupt Pin Register */

/* Device type values as ([PCI_BCR] << 16) | ([PCI_SCR] << 8) | [PCI_PIFR] */
#define PCI_T3_PCI2PCI          0x060400        /* PCI-to-PCI Bridge device */
#define PCI_T3_PCI2PCI_SUBTR    0x060401        /* Subtr. PCI-to-PCI Bridge */

/* PCI bridge devices (AGP) */
#define PPB_SBUSN       0x19    /* Secondary Bus Number */

/* Intel compatible PCI bridge devices (AGP) */
#define PPB_SSTS        0x1E    /* Secondary PCI-to-PCI Status Register */

#define PCI_NO_VID      0xffff  /* No PCI card present */
#define PCI_NO_DID      0xffff  /* No PCI card present */


void pci_init();

uint8_t  pci_r8 (unsigned int devidx, int reg);
uint16_t pci_r16(unsigned int devidx, int reg);
uint32_t pci_r32(unsigned int devidx, int reg);
void     pci_w16(unsigned int devidx, int reg, uint16_t value);
void     pci_w32(unsigned int devidx, int reg, uint32_t value);

bool pci_dev_find(uint8_t bus, uint8_t dev, uint8_t func, unsigned int * devidx);
bool pci_dev_first(unsigned int * devidx, uint16_t * vid, uint16_t * did);
bool pci_dev_next(unsigned int * devidx, uint16_t * vid, uint16_t * did);
void pci_dev_ids(unsigned int devidx, uint16_t * vid, uint16_t * did);


#endif
