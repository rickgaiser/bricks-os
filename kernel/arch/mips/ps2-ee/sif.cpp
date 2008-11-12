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


// Based on code from PS2SDK
#include "sif.h"
#include "dma.h"
#include "bios.h"
#include "stddef.h"
#include "asm/cpu.h"
#include "asm/arch/registers.h"


typedef signed int             int128_t   __attribute__((mode(TI)));
typedef unsigned int           uint128_t  __attribute__((mode(TI)));

typedef volatile signed int    vint128_t  __attribute__((mode(TI)));
typedef volatile unsigned int  vuint128_t __attribute__((mode(TI)));

// note: 'sync' is the same as 'sync.l'
#define EE_SYNC()  __asm__ volatile ("sync")
#define EE_SYNCL() __asm__ volatile ("sync.l")
#define EE_SYNCP() __asm__ volatile ("sync.p")

#define UNCACHED_SEG(x) ((void *)(((uint32_t)(x)) | 0x20000000))

#define SIF_REG_MAINADDR         1
#define SIF_REG_SUBADDR          2
#define SIF_REG_MSFLAG           3
#define SIF_REG_SMFLAG           4

#define CMD_PACKET_MAX         128
#define CMD_PACKET_DATA_MAX    112
#define CMD_HANDLER_MAX         32

// EE DMAC registers
#define CHCR_STR             0x100
#define STAT_SIF0             0x20


#define SYSTEM_CMD      0x80000000


struct SifCmdHeader_t
{
  uint32_t   size;
  void     * dest;
  int        cid;
  uint32_t   unknown;
};

typedef void (*SifCmdHandler_t)(void *, void *);

struct SifCmdHandlerData_t
{
  SifCmdHandler_t handler;
  void * harg;
};

struct ca_pkt
{
  SifCmdHeader_t header;
  void * buf;
};

struct sr_pkt
{
  SifCmdHeader_t header;
  uint32_t sreg;
  int val;
};

// Even though I'm reluctant to do this, I've made this structure binary
// compatible with the SCE libs and ps2lib.  In all implementations, a pointer
// to this data is stored in SIF register 0x80000001.  Each routine that
// relies on this data can then use the data referenced from that register, so
// that even if a SIF library is initialized after this one, we should still
// work exactly as expected.
struct cmd_data
{
  void                * pktbuf;           // Command packet received from the IOP
  void                * unused;           // Called "cmdbuf", but unused.
  void                * iopbuf;           // Address of IOP SIF DMA receive address
  SifCmdHandlerData_t * sys_cmd_handlers;
  uint32_t              nr_sys_handlers;
  SifCmdHandlerData_t * usr_cmd_handlers;
  uint32_t              nr_usr_handlers;
  int                 * sregs;
};

uint8_t pktbuf[128] __attribute__((aligned(64)));
// Define this so that in the unlikely case another SIF implementation decides
// to use it, it won't crash.  Otherwise unused.
uint8_t cmdbuf[64] __attribute__((aligned(64)));
SifCmdHandlerData_t sys_cmd_handlers[CMD_HANDLER_MAX];
SifCmdHandlerData_t usr_cmd_handlers[CMD_HANDLER_MAX];
int sregs[32];
cmd_data sCmdData =
{
  pktbuf,
  cmdbuf,
  NULL,
  sys_cmd_handlers,
  CMD_HANDLER_MAX,
  usr_cmd_handlers,
  CMD_HANDLER_MAX,
  sregs
};


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CIOP::CIOP()
 : bInitialized_(false)
 , iDMAHandlerID_(-1)
{
}

// -----------------------------------------------------------------------------
CIOP::~CIOP()
{
  if(bInitialized_ == true)
  {
    dmaShutdown(DMAC::Channel::SIF0);
    bInitialized_ = false;
  }
}

// -----------------------------------------------------------------------------
int
CIOP::init()
{
  uint32_t packet[5];  // Implicitly aligned to 16 bytes

  if(bInitialized_ == true)
    return 0;

  local_irq_disable();

  sCmdData.pktbuf = UNCACHED_SEG(sCmdData.pktbuf);
  sCmdData.unused = UNCACHED_SEG(sCmdData.unused);

  for(int i(0); i < CMD_HANDLER_MAX; i++)
  {
    sCmdData.sys_cmd_handlers[i].handler = NULL;
    sCmdData.sys_cmd_handlers[i].harg    = NULL;
  }

  for(int i(0); i < 32; i++)
    sCmdData.sregs[i] = 0;

  sCmdData.sys_cmd_handlers[0].handler = changeAddr;
  sCmdData.sys_cmd_handlers[0].harg    = &sCmdData;
  sCmdData.sys_cmd_handlers[1].handler = setSReg;
  sCmdData.sys_cmd_handlers[1].harg    = &sCmdData;

  local_irq_enable();
  bios::FlushCache(0);

  if(REG_DMA_STAT & STAT_SIF0)
    REG_DMA_STAT = STAT_SIF0;

  if(!(REG_DMA_SIF0_CHCR & CHCR_STR))
    bios::SifSetDChain();

  dmaInitialize(DMAC::Channel::SIF0, callBackHandler);
  bInitialized_ = true;

  sCmdData.iopbuf = (void *)bios::SifGetReg(0x80000000);
  if(sCmdData.iopbuf)
  {
    // IOP SIF CMD is already initialized, so give it our new receive address.
    ((struct ca_pkt *)(packet))->buf = sCmdData.pktbuf;
    sendCmd(0x80000000, packet, sizeof packet, NULL, NULL, 0);
  }
  else
  {
    // Sync
    while(!(bios::SifGetReg(SIF_REG_SMFLAG) & 0x20000));

    sCmdData.iopbuf = (void *)bios::SifGetReg(SIF_REG_SUBADDR);
    bios::SifSetReg(0x80000000, (uint32_t)sCmdData.iopbuf);
    // See the note above about struct cmd_data, and the use of this register.
    bios::SifSetReg(0x80000001, (uint32_t)&sCmdData);
    packet[3] = 0;
    packet[4] = (uint32_t)sCmdData.pktbuf;
    sendCmd(0x80000002, packet, sizeof packet, NULL, NULL, 0);
  }

  return 0;
}

// -----------------------------------------------------------------------------
bool
CIOP::alive()
{
  return ((bios::SifGetReg(SIF_REG_SMFLAG) & 0x10000) != 0);
}

// -----------------------------------------------------------------------------
bool
CIOP::sync()
{
  if(bios::SifGetReg(SIF_REG_SMFLAG) & 0x40000)
  {
    bios::SifSetReg(SIF_REG_SMFLAG, 0x40000);
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
uint32_t
CIOP::sendCmd(int cid, void * pkt, uint32_t pktsize, void * src, void * dest, int size)
{
  SifDmaTransfer_t dmat[2];
  SifCmdHeader_t *header;
  int count = 0;

  pktsize &= 0xff;

  if(pktsize > CMD_PACKET_DATA_MAX)
    return 0;

  header = (SifCmdHeader_t *)pkt;
  header->cid  = cid;
  header->size = pktsize;
  header->dest = NULL;

  if(size > 0)
  {
    header->size = pktsize | (size << 8);
    header->dest = dest;

    dmat[count].src  = src;
    dmat[count].dest = dest;
    dmat[count].size = size;
    dmat[count].attr = 0;
    count++;
  }

  dmat[count].src  = pkt;
  dmat[count].dest = sCmdData.iopbuf;
  dmat[count].size = pktsize;
  dmat[count].attr = 0x40 | SIF_DMA_INT_O;
  count++;

  bios::SifWriteBackDCache(pkt, pktsize);

  return bios::SifSetDma(dmat, count);
}

// -----------------------------------------------------------------------------
int32_t
CIOP::callBackHandler(int32_t channel)
{
  uint128_t   packet[8];
  uint128_t * pktbuf;
  struct cmd_data * cmd_data = &sCmdData;
  SifCmdHeader_t * header;
  SifCmdHandlerData_t * cmd_handlers;
  int size;
  int pktquads;
  int id;
  int i = 0;

  local_irq_enable();

  header = (SifCmdHeader_t *)cmd_data->pktbuf;
  size = (header->size & 0xff);

  if(size != 0)
  {
    // TODO: Don't copy anything extra
    pktquads = (size + 30) >> 4;
    header->size = 0;
    if(pktquads)
    {
      pktbuf = (uint128_t *)cmd_data->pktbuf;
      while(pktquads--)
      {
        packet[i] = pktbuf[i];
        i++;
      }
    }

    bios::iSifSetDChain();

    header = (SifCmdHeader_t *)packet;
    // Get the command handler id and determine which handler list to dispatch from.
    id = header->cid & ~SYSTEM_CMD;

    if(id < CMD_HANDLER_MAX)
    {
      if(header->cid & SYSTEM_CMD)
        cmd_handlers = cmd_data->sys_cmd_handlers;
      else
        cmd_handlers = cmd_data->usr_cmd_handlers;

      if(cmd_handlers[id].handler)
        cmd_handlers[id].handler(packet, cmd_handlers[id].harg);
    }
  }

  EE_SYNC();
  local_irq_enable();

  return 0;
}

// -----------------------------------------------------------------------------
void
CIOP::changeAddr(void * packet, void * harg)
{
  struct cmd_data * cmd_data = (struct cmd_data *)harg;
  struct ca_pkt *   pkt      = (struct ca_pkt *)packet;

  cmd_data->iopbuf = pkt->buf;
}

// -----------------------------------------------------------------------------
void
CIOP::setSReg(void * packet, void * harg)
{
  struct cmd_data * cmd_data = (struct cmd_data *)harg;
  struct sr_pkt *   pkt      = (struct sr_pkt *)packet;

  cmd_data->sregs[pkt->sreg] = pkt->val;
}
