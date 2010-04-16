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


#include "pci.h"
#include "asm/hal.h"
#include "kernel/debug.h"


#define PCII_CONFADD                 0xCF8
#define PCIIC_CODE              0x80000000
#define PCIIC_BUSNUM_MASK         0xff0000
#define PCIIC_BUSNUM_SHIFT              16
#define PCIIC_DEVNUM_MASK           0xf800
#define PCIIC_DEVNUM_SHIFT              11
#define PCIIC_FUNCNUM_MASK           0x700
#define PCIIC_FUNCNUM_SHIFT              8
#define PCIIC_REGNUM_MASK             0xfc
#define PCIIC_REGNUM_SHIFT               2
#define PCII_CONFDATA                0xCFC

#define PCII_SELREG_(bus, dev, func, reg) \
  (PCIIC_CODE | \
  (((bus) << PCIIC_BUSNUM_SHIFT) & PCIIC_BUSNUM_MASK) | \
  (((dev) << PCIIC_DEVNUM_SHIFT) & PCIIC_DEVNUM_MASK) | \
  (((func) << PCIIC_FUNCNUM_SHIFT) & PCIIC_FUNCNUM_MASK) | \
  ((((reg)/4) << PCIIC_REGNUM_SHIFT) & PCIIC_REGNUM_MASK))
#define PCII_UNSEL      (0)

#define PCII_RREG8_(bus, dev, func, reg) \
  (outd(PCII_SELREG_(bus, dev, func, reg), PCII_CONFADD), \
  inb(PCII_CONFDATA+((reg)&3)))
#define PCII_RREG16_(bus, dev, func, reg) \
  (PCII_RREG8_(bus, dev, func, reg) | \
  (PCII_RREG8_(bus, dev, func, reg+1) << 8))
#define PCII_RREG32_(bus, dev, func, reg) \
  (PCII_RREG16_(bus, dev, func, reg) | \
  (PCII_RREG16_(bus, dev, func, reg+2) << 16))

#define PCII_WREG8_(bus, dev, func, reg, val) \
  (outd(PCII_SELREG_(bus, dev, func, reg), PCII_CONFADD), \
  outb((val), PCII_CONFDATA+((reg)&3)))
#define PCII_WREG16_(bus, dev, func, reg, val) \
  (PCII_WREG8_(bus, dev, func, reg, (val)), \
  (PCII_WREG8_(bus, dev, func, reg+1, (val) >> 8)))
#define PCII_WREG32_(bus, dev, func, reg, val) \
  (PCII_WREG16_(bus, dev, func, reg, (val)), \
  (PCII_WREG16_(bus, dev, func, reg+2, (val) >> 16)))

#define VALID_DEVICE_IDX(idx) \
  (idx < PCI_MAX_DEVICES)


struct pci_vendor
{
  uint16_t vid;
  const char * name;
};

struct pci_device
{
  uint16_t vid;
  uint16_t did;
  const char * name;
};

struct pci_baseclass
{
  uint8_t baseclass;
  const char * name;
};

struct pci_subclass
{
  uint8_t baseclass;
  uint8_t subclass;
  uint16_t infclass;
  const char * name;
};

struct pci_isabridge
{
  uint16_t vid;
  uint16_t did;
  int checkclass;
  int type;
};

struct pci_pcibridge
{
  uint16_t vid;
  uint16_t did;
  int type;
};

#define PCI_IB_PIIX     1       /* Intel PIIX compatible ISA bridge */
#define PCI_IB_VIA      2       /* VIA compatible ISA bridge */
#define PCI_IB_AMD      3       /* AMD compatible ISA bridge */
#define PCI_IB_SIS      4       /* SIS compatible ISA bridge */

#define PCI_PCIB_INTEL  1       /* Intel compatible PCI bridge */
#define PCI_AGPB_INTEL  2       /* Intel compatible AGP bridge */
#define PCI_AGPB_VIA    3       /* VIA compatible AGP bridge */

struct pci_vendor pci_vendor_table[]=
{
  { 0x1000, "NCR" },
  { 0x1002, "ATI Technologies" },
  { 0x100B, "National Semiconductor Corporation" },
  { 0x1013, "Cirrus Logic" },
  { 0x1022, "Advanced Micro Devices" },
  { 0x102B, "Matrox Graphics, Inc." },
  { 0x1039, "Silicon Integrated Systems (SiS)" },
  { 0x104C, "Texas Instruments" },
  { 0x105A, "Promise Technology" },
  { 0x10B7, "3Com Corporation" },
  { 0x10B9, "AcerLabs (ALI)" },
  { 0x10DE, "nVidia Corporation" },
  { 0x10EC, "Realtek" },
  { 0x1106, "VIA" },
  { 0x110A, "Siemens Nixdorf AG" },
  { 0x125D, "ESS Technology" },
  { 0x1274, "Ensoniq" },
  { 0x104b, "Mylex / Buslogic" },
  { 0x15ad, "VMware Inc." },
  { 0x5333, "S3" },
  { 0x8086, "Intel" },
  { 0x9004, "Adaptec" },
  { 0x9005, "Adaptec" },
  { 0x0000, NULL }
};

struct pci_device pci_device_table[]=
  {
  { 0x1000, 0x0001, "NCR 53C810" },
  { 0x1000, 0x000F, "NCR 53C875" },
  { 0x1002, 0x4752, "ATI Rage XL PCI" },
  { 0x100B, 0xD001, "Nat. Semi. 87410" },
  { 0x1013, 0x00B8, "Cirrus Logic GD 5446" },
  { 0x1013, 0x6003, "Cirrus Logic CS4614/22/24 CrystalClear" },
  { 0x1022, 0x1100, "K8 HyperTransport Tech. Conf." },
  { 0x1022, 0x1101, "K8 [Athlon64/Opteron] Address Map" },
  { 0x1022, 0x1102, "K8 [Athlon64/Opteron] DRAM Controller" },
  { 0x1022, 0x1103, "K8 [Athlon64/Opteron] Misc. Control" },
  { 0x1022, 0x2000, "AMD Lance/PCI" },
  { 0x1022, 0x700C, "AMD-762 CPU to PCI Bridge (SMP chipset)" },
  { 0x1022, 0x700D, "AMD-762 CPU to PCI Bridge (AGP 4x)" },
  { 0x1022, 0x7410, "AMD-766 PCI to ISA/LPC Bridge" },
  { 0x1022, 0x7411, "AMD-766 EIDE Controller" },
  { 0x102B, 0x051B, "Matrox MGA 2164W [Millennium II]" },
  { 0x102B, 0x0525, "Matrox MGA G400 AGP" },
  { 0x1039, 0x0008, "SiS 85C503/5513" },
  { 0x1039, 0x0200, "SiS 5597/5598 VGA" },
  { 0x1039, 0x0406, "SiS 85C501/2" },
  { 0x1039, 0x5597, "SiS 5582" },
  { 0x104b, 0x1040, "BT958 SCSI Host Adaptor" },
  { 0x104C, 0xAC1C, "TI PCI1225" },
  { 0x105A, 0x0D30, "Promise Technology 20265" },
  { 0x10B7, 0x9058, "3Com 3c905B-Combo" },
  { 0x10B7, 0x9805, "3Com 3c980-TX Python-T" },
  { 0x10B9, 0x1533, "ALI M1533 ISA-bridge [Aladdin IV]" },
  { 0x10B9, 0x1541, "ALI M1541" },
  { 0x10B9, 0x5229, "ALI M5229 (IDE)" },
  { 0x10B9, 0x5243, "ALI M5243" },
  { 0x10B9, 0x7101, "ALI M7101 PMU" },
  { 0x10DE, 0x0020, "nVidia Riva TnT [NV04]" },
  { 0x10DE, 0x0110, "nVidia GeForce2 MX [NV11]" },
  { 0x10EC, 0x8029, "Realtek RTL8029" },
  { 0x10EC, 0x8139, "Realtek RTL8139" },
  { 0x1106, 0x0305, "VIA VT8363/8365 [KT133/KM133]" },
  { 0x1106, 0x0571, "VIA IDE controller" },
  { 0x1106, 0x0686, "VIA VT82C686 (Apollo South Bridge)" },
  { 0x1106, 0x1204, "K8M800 Host Bridge" },
  { 0x1106, 0x2204, "K8M800 Host Bridge" },
  { 0x1106, 0x3038, "VT83C572 PCI USB Controller" },
  { 0x1106, 0x3057, "VT82C686A ACPI Power Management Controller" },
  { 0x1106, 0x3058, "VIA AC97 Audio Controller" },
  { 0x1106, 0x3059, "VIA AC97 Audio Controller" },
  { 0x1106, 0x3065, "VT6102 [Rhine-II]" },
  { 0x1106, 0x3074, "VIA VT8233" },
  { 0x1106, 0x3099, "VIA VT8367 [KT266]" },
  { 0x1106, 0x3104, "VIA USB 2.0" },
  { 0x1106, 0x3108, "VIA S3 Unichrome Pro VGA Adapter" },
  { 0x1106, 0x3149, "VIA VT6420 SATA RAID Controller" },
  { 0x1106, 0x3204, "K8M800 Host Bridge" },
  { 0x1106, 0x3227, "VT8237 ISA bridge" },
  { 0x1106, 0x4204, "K8M800 Host Bridge" },
  { 0x1106, 0x8305, "VIA VT8365 [KM133 AGP]" },
  { 0x1106, 0xB099, "VIA VT8367 [KT266 AGP]" },
  { 0x1106, 0xB188, "VT8237 PCI bridge" },
  { 0x110A, 0x0005, "Siemens Nixdorf Tulip Cntlr., Power Management" },
  { 0x1186, 0x1300, "D-Link RTL8139" },
  { 0x125D, 0x1969, "ESS ES1969 Solo-1 Audiodrive" },
  { 0x1274, 0x1371, "Ensoniq ES1371 [AudioPCI-97]" },
  { 0x1274, 0x5000, "Ensoniq ES1370" },
  { 0x1274, 0x5880, "Ensoniq CT5880 [AudioPCI]" },
  { 0x15ad, 0x0405, "VMware SVGA II Adapter" },
  { 0x15ad, 0x0790, "VMware PCI Bridge" },
  { 0x15ad, 0x07a0, "VMware PCI Express Root Port" },
  { 0x5333, 0x8811, "S3 86c764/765 [Trio32/64/64V+]" },
  { 0x5333, 0x883d, "S3 Virge/VX" },
  { 0x5333, 0x88d0, "S3 Vision 964 vers 0" },
  { 0x5333, 0x8a01, "S3 Virge/DX or /GX" },
  { 0x8086, 0x1004, "Intel 82543GC Gigabit Ethernet Controller" },
  { 0x8086, 0x1029, "Intel EtherExpressPro100 ID1029" },
  { 0x8086, 0x1030, "Intel Corporation 82559 InBusiness 10/100" },
  { 0x8086, 0x1209, "Intel EtherExpressPro100 82559ER" },
  { 0x8086, 0x1229, "Intel EtherExpressPro100 82557/8/9" },
  { 0x8086, 0x122D, "Intel 82437FX" },
  { 0x8086, 0x122E, "Intel 82371FB (PIIX)" },
  { 0x8086, 0x1230, "Intel 82371FB (IDE)" },
  { 0x8086, 0x1237, "Intel 82441FX (440FX)" },
  { 0x8086, 0x1250, "Intel 82439HX" },
  { 0x8086, 0x1A30, "Intel 82845B/A MCH" },
  { 0x8086, 0x1A31, "Intel 82845B/A PCI Bridge to AGP port" },
  { 0x8086, 0x2440, "Intel 82801B PCI to ISA bridge" },
  { 0x8086, 0x2449, "Intel EtherExpressPro100 82562EM" },
  { 0x8086, 0x244e, "Intel 82801 PCI Bridge" },
  { 0x8086, 0x2560, "Intel 82845G/GL[Brookdale-G]/GE/PE" },
  { 0x8086, 0x2561, "Intel 82845G/GL/GE/PE Host-to-AGP Bridge" },
  { 0x8086, 0x7000, "Intel 82371SB" },
  { 0x8086, 0x7010, "Intel 82371SB (IDE)" },
  { 0x8086, 0x7020, "Intel 82371SB (USB)" },
  { 0x8086, 0x7030, "Intel 82437VX" },    /* asw 2005-03-02 */
  { 0x8086, 0x7100, "Intel 82371AB" },    /* asw 2004-07-31 */
  { 0x8086, 0x7100, "Intel 82371AB" },
  { 0x8086, 0x7110, "Intel 82371AB (PIIX4)" },
  { 0x8086, 0x7111, "Intel 82371AB (IDE)" },
  { 0x8086, 0x7112, "Intel 82371AB (USB)" },
  { 0x8086, 0x7113, "Intel 82371AB (Power)" },
  { 0x8086, 0x7124, "Intel 82801AA" },    /* asw 2004-11-09 */
  { 0x8086, 0x7190, "Intel 82443BX" },
  { 0x8086, 0x7191, "Intel 82443BX (AGP bridge)" },
  { 0x8086, 0x7192, "Intel 82443BX (Host-to-PCI bridge)" },
  { 0x9004, 0x8178, "Adaptec AHA-2940U/2940UW Ultra/Ultra-Wide SCSI Ctrlr" },
  { 0x9005, 0x0080, "Adaptec AIC-7892A Ultra160/m PCI SCSI Controller" },
  { 0x0000, 0x0000, NULL }
};

struct pci_baseclass pci_baseclass_table[]=
  {
  { 0x00, "No device class" },
  { 0x01, "Mass storage controller" },
  { 0x02, "Network controller" },
  { 0x03, "Display controller" },
  { 0x04, "Multimedia device" },
  { 0x05, "Memory controller" },
  { 0x06, "Bridge device" },
  { 0x07, "Simple comm. controller" },
  { 0x08, "Base system peripheral" },
  { 0x09, "Input device" },
  { 0x0A, "Docking station" },
  { 0x0B, "Processor" },
  { 0x0C, "Serial bus controller" },
  { 0x0d, "Wireless controller" },
  { 0x0e, "Intelligent I/O controller" },
  { 0x0f, "Satellite comm. controller" },
  { 0x10, "Encryption/decryption controller" },
  { 0x11, "Data acquisition controller" },
  { 0xff, "Misc. device" },

  { 0x00, NULL }
};

  /* -1 in the infclass field is a wildcard for infclass */
struct pci_subclass pci_subclass_table[]=
  {
  { 0x00, 0x01, 0x00, "VGA-compatible device" },

  { 0x01, 0x00, 0x00, "SCSI bus controller" },
  { 0x01, 0x01, -1,   "IDE controller" },
  { 0x01, 0x02, 0x00, "Floppy disk controller" },
  { 0x01, 0x03, 0x00, "IPI controller" },
  { 0x01, 0x04, 0x00, "RAID controller" },
  { 0x01, 0x80, 0x00, "Other mass storage controller" },

  { 0x02, 0x00, 0x00, "Ethernet controller" },
  { 0x02, 0x01, 0x00, "Token Ring controller" },
  { 0x02, 0x02, 0x00, "FDDI controller" },
  { 0x02, 0x03, 0x00, "ATM controller" },
  { 0x02, 0x04, 0x00, "ISDN controller" },
  { 0x02, 0x80, 0x00, "Other network controller" },

  { 0x03, 0x00, 0x00, "VGA-compatible controller" },
  { 0x03, 0x00, 0x01, "8514-compatible controller" },
  { 0x03, 0x01, 0x00, "XGA controller" },
  { 0x03, 0x02, 0x00, "3D controller" },
  { 0x03, 0x80, 0x00, "Other display controller" },

  { 0x04, 0x00, 0x00, "Video device" },
  { 0x04, 0x01, 0x00, "Audio device" },
  { 0x04, 0x02, 0x00, "Computer telephony device" },
  { 0x04, 0x80, 0x00, "Other multimedia device" },

  { 0x06, 0x00, 0x00, "Host bridge" },
  { 0x06, 0x01, 0x00, "ISA bridge" },
  { 0x06, 0x02, 0x00, "EISA bridge" },
  { 0x06, 0x03, 0x00, "MCA bridge" },
  { 0x06, 0x04, 0x00, "PCI-to-PCI bridge" },
  { 0x06, 0x04, 0x01, "Subtractive decode PCI-to-PCI bridge" },
  { 0x06, 0x05, 0x00, "PCMCIA bridge" },
  { 0x06, 0x06, 0x00, "NuBus bridge" },
  { 0x06, 0x07, 0x00, "CardBus bridge" },
  { 0x06, 0x08, -1,   "RACEway bridge" },
  { 0x06, 0x09, -1,   "Semi-transparent PCI-to-PCI bridge" },
  { 0x06, 0x80, 0x00, "Other bridge device" },

  { 0x0C, 0x00, 0x00, "IEEE 1394 (FireWire)" },
  { 0x0C, 0x00, 0x10, "IEEE 1394 (OpenHCI)" },
  { 0x0C, 0x01, 0x00, "ACCESS bus" },
  { 0x0C, 0x02, 0x00, "SSA" },
  { 0x0C, 0x03, 0x00, "USB (with UHC)" },
  { 0x0C, 0x03, 0x10, "USB (with OHC)" },
  { 0x0C, 0x03, 0x80, "USB (other host inf.)" },
  { 0x0C, 0x03, 0xFE, "USB device" },
  { 0x0C, 0x04, 0x00, "Fibre Channel" },
  { 0x0C, 0x05, 0x00, "SMBus" },

  { 0x00, 0x00, 0x00, NULL }
};

struct pci_isabridge pci_isabridge[]=
{
  { 0x1022, 0x7410, 1, PCI_IB_AMD,        },      /* AMD-766 */
  { 0x1039, 0x0008, 1, PCI_IB_SIS,        },      /* SiS 85C503/5513 */
  { 0x10B9, 0x1533, 1, PCI_IB_PIIX,       },      /* ALI M1533 */
  { 0x1106, 0x0686, 1, PCI_IB_VIA,        },      /* VIA VT82C686 */
  { 0x1106, 0x3074, 1, PCI_IB_VIA,        },      /* VIA VT8233 */
  { 0x1106, 0x3227, 1, PCI_IB_VIA,        },      /* VIA */
  { 0x8086, 0x122E, 1, PCI_IB_PIIX,       },      /* Intel 82371FB */
  { 0x8086, 0x2440, 1, PCI_IB_PIIX,       },      /* Intel 82801B */
  { 0x8086, 0x7000, 1, PCI_IB_PIIX,       },      /* Intel 82371SB */
  { 0x8086, 0x7030, 1, PCI_IB_PIIX,       },      /* Intel 82437VX (asw 2005-03-02) */
  { 0x8086, 0x7100, 1, PCI_IB_PIIX,       },      /* Intel 82371AB (asw 2004-07-31) */
  { 0x8086, 0x7110, 1, PCI_IB_PIIX,       },      /* Intel PIIX4 */
  { 0x8086, 0x7124, 1, PCI_IB_PIIX,       },      /* Intel 82801AA (asw 2004-11-09) */
  { 0x0000, 0x0000, 0, 0,                 },
};

struct pci_pcibridge pci_pcibridge[]=
{
  { 0x8086, 0x1A31, PCI_AGPB_INTEL, },    /* Intel 82845B/A AGP Bridge */
  { 0x8086, 0x2448, PCI_PCIB_INTEL, },    /* Intel 82801 Mobile */
  { 0x8086, 0x244e, PCI_PCIB_INTEL, },    /* Intel 82801 PCI Bridge */
  { 0x8086, 0x2561, PCI_AGPB_INTEL, },    /* Intel 82845 AGP Bridge */
  { 0x8086, 0x7191, PCI_AGPB_INTEL, },    /* Intel 82443BX (AGP bridge) */
  { 0x1022, 0x700D, PCI_AGPB_INTEL, },    /* AMD-762 (AGP 4x) */
  { 0x10B9, 0x5243, PCI_AGPB_INTEL, },    /* ALI M5243 */
  { 0x1106, 0x8305, PCI_AGPB_VIA, },      /* VIA VT8365 [KM133 AGP] */
  { 0x1106, 0xB188, PCI_AGPB_VIA, },      /* VT8237 PCI bridge */
  { 0x0000, 0x0000, 0, },
};

struct SPCIBus
{
  int iBusNr;
};

struct SPCIDevice
{
  int iBusIdx;
  int iDevNr;
  int iFuncNr;

  uint16_t vid;
  uint16_t did;
  uint8_t iBaseClass;
  uint8_t iSubClass;
  uint8_t iInfClass;

  bool bPresent;
};

#define PCI_MAX_BUSSES 4
#define PCI_MAX_DEVICES 40
SPCIBus pcibus[PCI_MAX_BUSSES];
SPCIDevice pcidev[PCI_MAX_DEVICES];
int iCurrentBusIdx = 0;
int iCurrentDevIdx = 0;

void pci_probe_bus(unsigned int busidx);


// -----------------------------------------------------------------------------
const char *
getVIDString(uint16_t vid)
{
  for(int i = 0; pci_vendor_table[i].name != NULL; i++)
  {
    if(pci_vendor_table[i].vid == vid)
    {
      return pci_vendor_table[i].name;
    }
  }
  return "unknown";
}

// -----------------------------------------------------------------------------
const char *
getDIDString(uint16_t vid, uint16_t did)
{
  for(int i = 0; pci_device_table[i].name != NULL; i++)
  {
    if((pci_device_table[i].vid == vid) &&
       (pci_device_table[i].did == did))
    {
      return pci_device_table[i].name;
    }
  }
  return "unknown";
}

// -----------------------------------------------------------------------------
const char *
getBaseClassString(uint8_t baseclass)
{
  for(int i = 0; pci_baseclass_table[i].name != NULL; i++)
  {
    if(pci_baseclass_table[i].baseclass == baseclass)
    {
      return pci_baseclass_table[i].name;
    }
  }
  return "unknown";
}

// -----------------------------------------------------------------------------
const char *
getSubClassString(uint8_t baseclass, uint8_t subclass, uint8_t infclass)
{
  for(int i = 0; pci_subclass_table[i].name != NULL; i++)
  {
    if((pci_subclass_table[i].baseclass == baseclass) &&
       (pci_subclass_table[i].subclass == subclass) &&
       ((pci_subclass_table[i].infclass == infclass) || (pci_subclass_table[i].infclass == 0xffff)))
    {
      return pci_subclass_table[i].name;
    }
  }
  return "unknown";
}

// -----------------------------------------------------------------------------
uint8_t
pci_r8(unsigned int devidx, int reg)
{
  uint8_t iRetVal = 0;

  if(VALID_DEVICE_IDX(devidx))
  {
    iRetVal = PCII_RREG8_(pcibus[pcidev[devidx].iBusIdx].iBusNr, pcidev[devidx].iDevNr, pcidev[devidx].iFuncNr, reg);
    outd(PCII_UNSEL, PCII_CONFADD);
  }

  return iRetVal;
}

// -----------------------------------------------------------------------------
uint16_t
pci_r16(unsigned int devidx, int reg)
{
  uint16_t iRetVal = 0;

  if(VALID_DEVICE_IDX(devidx))
  {
    iRetVal = PCII_RREG16_(pcibus[pcidev[devidx].iBusIdx].iBusNr, pcidev[devidx].iDevNr, pcidev[devidx].iFuncNr, reg);
    outd(PCII_UNSEL, PCII_CONFADD);
  }

  return iRetVal;
}

// -----------------------------------------------------------------------------
uint32_t
pci_r32(unsigned int devidx, int reg)
{
  uint32_t iRetVal = 0;

  if(VALID_DEVICE_IDX(devidx))
  {
    iRetVal = PCII_RREG32_(pcibus[pcidev[devidx].iBusIdx].iBusNr, pcidev[devidx].iDevNr, pcidev[devidx].iFuncNr, reg);
    outd(PCII_UNSEL, PCII_CONFADD);
  }

  return iRetVal;
}

// -----------------------------------------------------------------------------
void
pci_w16(unsigned int devidx, int reg, uint16_t value)
{
  if(VALID_DEVICE_IDX(devidx))
  {
    PCII_WREG16_(pcibus[pcidev[devidx].iBusIdx].iBusNr, pcidev[devidx].iDevNr, pcidev[devidx].iFuncNr, reg, value);
    outd(PCII_UNSEL, PCII_CONFADD);
  }
}

// -----------------------------------------------------------------------------
void
pci_w32(unsigned int devidx, int reg, uint32_t value)
{
  if(VALID_DEVICE_IDX(devidx))
  {
    PCII_WREG32_(pcibus[pcidev[devidx].iBusIdx].iBusNr, pcidev[devidx].iDevNr, pcidev[devidx].iFuncNr, reg, value);
    outd(PCII_UNSEL, PCII_CONFADD);
  }
}

// -----------------------------------------------------------------------------
void
pci_add_isa_bridge(unsigned int devidx)
{
/*
  // Find out what type of bridge this is
  for(int i = 0; pci_isabridge[i].vid != 0; i++)
  {
    if((pcidev[devidx].vid == pci_isabridge[i].vid) &&
       (pcidev[devidx].did == pci_isabridge[i].did))
    {
      switch(pci_isabridge[i].type)
      {
        case PCI_IB_PIIX:
        //  pci_add_isa_bridge_piix(devidx);
        //  break;
        case PCI_IB_VIA:
        //  pci_add_isa_bridge_via(devidx);
        //  break;
        case PCI_IB_AMD:
        //  pci_add_isa_bridge_amd(devidx);
        //  break;
        case PCI_IB_SIS:
        //  pci_add_isa_bridge_sis(devidx);
        //  break;
        default:
          printk("ISA Bridge not supported\n");
          break;
      };

      return;
    }
  }
*/
}

// -----------------------------------------------------------------------------
void
pci_add_pci_bridge(unsigned int devidx)
{
  printk(" - PCI bus found: 0x%x:0x%x - %s:%s\n", pcidev[devidx].vid, pcidev[devidx].did, getVIDString(pcidev[devidx].vid), getDIDString(pcidev[devidx].vid, pcidev[devidx].did));

  if(iCurrentBusIdx >= PCI_MAX_BUSSES)
  {
    printk("ERROR: Too many PCI busses, bus not used!\n");
    return;
  }

  pcibus[iCurrentBusIdx].iBusNr = pci_r8(devidx, PPB_SBUSN);
  pci_probe_bus(iCurrentBusIdx);
  iCurrentBusIdx++;
  return;

/*
  // Find out what type of bridge this is
  for(int i = 0; pci_pcibridge[i].vid != 0; i++)
  {
    if((pcidev[devidx].vid == pci_pcibridge[i].vid) &&
       (pcidev[devidx].did == pci_pcibridge[i].did))
    {
      switch(pci_pcibridge[i].type)
      {
        case PCI_PCIB_INTEL:
        //  pci_add_pci_bridge_pcib_intel(devidx);
        //  break;
        case PCI_AGPB_INTEL:
        //  pci_add_pci_bridge_agpb_intel(devidx);
        //  break;
        case PCI_AGPB_VIA:
        //  pci_add_pci_bridge_agpb_via(devidx);
        //  break;
        default:
          printk("PCI Bridge not supported\n");
          break;
      };
      return;
    }
  }
*/
}

// -----------------------------------------------------------------------------
void
pci_probe_bus(unsigned int busidx)
{
  uint16_t vid;
  uint16_t did;
  uint16_t sts;

  for(int dev = 0; dev < 32; dev++)
  {
    for(int func = 0; func < 8; func++)
    {
      // Create device object
      pcidev[iCurrentDevIdx].iBusIdx = busidx;
      pcidev[iCurrentDevIdx].iDevNr = dev;
      pcidev[iCurrentDevIdx].iFuncNr = func;

      pci_w16(iCurrentDevIdx, PPB_SSTS, PSR_SSE|PSR_RMAS|PSR_RTAS);
      vid = pci_r16(iCurrentDevIdx, PCI_VID);
      did = pci_r16(iCurrentDevIdx, PCI_DID);
      sts = pci_r16(iCurrentDevIdx, PPB_SSTS);

      if((vid == PCI_NO_VID) || (did == PCI_NO_DID))
        break;

      //if(sts & (PSR_SSE|PSR_RMAS|PSR_RTAS))
      //  break;

      printk(" - PCI device found: %d:%d:%d - 0x%x:0x%x - %s:%s\n", pcibus[busidx].iBusNr, dev, func, vid, did, getVIDString(vid), getDIDString(vid, did));

      if(iCurrentDevIdx >= PCI_MAX_DEVICES)
      {
        printk("ERROR: Too many PCI devices, device not used!\n");
        continue;
      }

      // Create device object
      pcidev[iCurrentDevIdx].vid = vid;
      pcidev[iCurrentDevIdx].did = did;
      pcidev[iCurrentDevIdx].iBaseClass = pci_r8(iCurrentDevIdx, PCI_BCR);;
      pcidev[iCurrentDevIdx].iSubClass = pci_r8(iCurrentDevIdx, PCI_SCR);
      pcidev[iCurrentDevIdx].iInfClass = pci_r8(iCurrentDevIdx, PCI_PIFR);
      pcidev[iCurrentDevIdx].bPresent = true;

      //printk("   - Location:   %d:%d:%d\n", pcibus[busidx].iBusNr, dev, func);
      //printk("   - Vendor:     0x%x - %s\n", vid, getVIDString(vid));
      //printk("   - Device:     0x%x - %s\n", did, getDIDString(vid, did));
      printk("   - Base Class: 0x%x - %s\n", pcidev[iCurrentDevIdx].iBaseClass, getBaseClassString(pcidev[iCurrentDevIdx].iBaseClass));
      printk("   - Sub Class:  0x%x - %s\n", pcidev[iCurrentDevIdx].iSubClass, getSubClassString(pcidev[iCurrentDevIdx].iBaseClass, pcidev[iCurrentDevIdx].iSubClass, pcidev[iCurrentDevIdx].iInfClass));

      // ISA Bridge
      if((pcidev[iCurrentDevIdx].iBaseClass == 0x06) &&
         (pcidev[iCurrentDevIdx].iSubClass == 0x01) &&
         (pcidev[iCurrentDevIdx].iInfClass == 0x00))
      {
        pci_add_isa_bridge(iCurrentDevIdx);
      }

      // PCI Bridge
      if((pcidev[iCurrentDevIdx].iBaseClass == 0x06) &&
         (pcidev[iCurrentDevIdx].iSubClass == 0x04) &&
         (pcidev[iCurrentDevIdx].iInfClass == 0x00))
      {
        pci_add_pci_bridge(iCurrentDevIdx);
      }

      iCurrentDevIdx++;
    }
  }
}

// -----------------------------------------------------------------------------
void
pci_init()
{
  printk("PCI bus scan:\n");

  uint16_t vid = PCII_RREG16_(0, 0, 0, PCI_VID);
  uint16_t did = PCII_RREG16_(0, 0, 0, PCI_DID);
  outd(PCII_UNSEL, PCII_CONFADD);

  if((vid != PCI_NO_VID)/* || (did != PCI_NO_DID)*/)
  {
    printk(" - PCI bus found: 0x%x:0x%x - %s:%s\n", vid, did, getVIDString(vid), getDIDString(vid, did));

    if(iCurrentBusIdx >= PCI_MAX_BUSSES)
    {
      printk("ERROR: Too many PCI busses, bus not used!\n");
      return;
    }

    // Create bus object
    pcibus[iCurrentBusIdx].iBusNr = 0;
    pci_probe_bus(iCurrentBusIdx);
    iCurrentBusIdx++;
  }

  printk(" - Done\n");
}

// -----------------------------------------------------------------------------
bool
pci_dev_find(uint8_t bus, uint8_t dev, uint8_t func, unsigned int * devidx)
{
  for(unsigned int i = 0; i < PCI_MAX_DEVICES; i++)
  {
    if((pcidev[i].bPresent == true) &&
        (pcibus[pcidev[i].iBusIdx].iBusNr == bus) &&
        (pcidev[i].iDevNr == dev) &&
        (pcidev[i].iFuncNr == func))
    {
      *devidx = i;
      return true;
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
bool
pci_dev_first(unsigned int * devidx, uint16_t * vid, uint16_t * did)
{
  for(unsigned int i = 0; i < PCI_MAX_DEVICES; i++)
  {
    if(pcidev[i].bPresent == true)
    {
      *devidx = i;
      *vid = pcidev[i].vid;
      *did = pcidev[i].did;
      return true;
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
bool
pci_dev_next(unsigned int * devidx, uint16_t * vid, uint16_t * did)
{
  for(unsigned int i = *devidx+1; i < PCI_MAX_DEVICES; i++)
  {
    if(pcidev[i].bPresent == true)
    {
      *devidx = i;
      *vid = pcidev[i].vid;
      *did = pcidev[i].did;
      return true;
    }
  }

  return false;
}

// -----------------------------------------------------------------------------
void
pci_dev_ids(unsigned int devidx, uint16_t * vid, uint16_t * did)
{
  if(VALID_DEVICE_IDX(devidx))
  {
    *vid = pcidev[devidx].vid;
    *did = pcidev[devidx].did;
  }
}
