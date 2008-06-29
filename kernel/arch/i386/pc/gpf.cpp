#include "gpf.h"
#include "asm/arch/config.h"
#include "asm/hal.h"
#include "asm/task.h"
#include "descriptor.h"
#include "kernel/task.h"
#include "kernel/debug.h"


extern STaskStateSegment * pCurrentTSS;
CThreadImpl * pGPFThread;


// -----------------------------------------------------------------------------
uint8_t
v86_fetch8(STaskStateSegment & tss)
{
  uint8_t data;

  data = *(uint8_t *)from_v86_addr(tss.cs, tss.eip);
  tss.eip++;

  return data;
}

// -----------------------------------------------------------------------------
void
v86_push16(STaskStateSegment & tss, uint16_t data)
{
  tss.esp = (tss.esp - 2) & 0xffff;
  *(uint16_t *)from_v86_addr(tss.ss, tss.esp) = data;
}

// -----------------------------------------------------------------------------
uint16_t
v86_pop16(STaskStateSegment & tss)
{
  uint16_t data;

  data = *(uint16_t *)from_v86_addr(tss.ss, tss.esp);
  tss.esp = (tss.esp + 2) & 0xffff;

  return data;
}

// -----------------------------------------------------------------------------
void
v86_push32(STaskStateSegment & tss, uint32_t data)
{
  tss.esp = (tss.esp - 4) & 0xffff;
  *(uint32_t *)from_v86_addr(tss.ss, tss.esp) = data;
}

// -----------------------------------------------------------------------------
uint32_t
v86_pop32(STaskStateSegment & tss)
{
  uint32_t data;

  data = *(uint32_t *)from_v86_addr(tss.ss, tss.esp);
  tss.esp = (tss.esp + 4) & 0xffff;

  return data;
}

// -----------------------------------------------------------------------------
#define PFX_ES    0x001
#define PFX_CS    0x002
#define PFX_SS    0x004
#define PFX_DS    0x008
#define PFX_FS    0x010
#define PFX_GS    0x020
#define PFX_OP32  0x040
#define PFX_ADR32 0x080
#define PFX_LOCK  0x100
#define PFX_REPNE 0x200
#define PFX_REP   0x400
// -----------------------------------------------------------------------------
int
v86gpf_handler(STaskStateSegment & tss)
{
  int      iRetVal(0);
  uint32_t prefix(0);
  bool     bPrefixLoop(true);
  uint8_t  data;

  while(bPrefixLoop)
  {
    data = v86_fetch8(tss);
    switch(data)
    {
      case 0x26:
        prefix |= PFX_ES;
        break;
      case 0x2e:
        prefix |= PFX_CS;
        break;
      case 0x36:
        prefix |= PFX_SS;
        break;
      case 0x3e:
        prefix |= PFX_DS;
        break;
      case 0x64:
        prefix |= PFX_FS;
        break;
      case 0x65:
        prefix |= PFX_GS;
        break;
      case 0x66:
        prefix |= PFX_OP32;
        break;
      case 0x67:
        prefix |= PFX_ADR32;
        break;
      case 0xf0:
        prefix |= PFX_LOCK;
        break;
      case 0xf2:
        prefix |= PFX_REPNE;
        break;
      case 0xf3:
        prefix |= PFX_REP;
        break;
      default:
        bPrefixLoop = false;
    };
  }

  switch(data)
  {
    case 0xcd: // int xx
    {
      uint16_t * base = 0;
      data = v86_fetch8(tss);
      // push eip, cs, eflags
      v86_push16(tss, tss.eflags);
      v86_push16(tss, tss.cs);
      v86_push16(tss, tss.eip);
      // int xx
      tss.cs  = base[data * 2 + 1];
      tss.eip = base[data * 2 + 0];
      break;
    }
    case 0xcf: // iret
    {
      // pop eip, cs, eflags
      if(prefix & PFX_OP32)
      {
        if(tss.esp > 0x0ff4)
          iRetVal = 1; // Exit
        tss.eip    = v86_pop32(tss);
        tss.cs     = v86_pop32(tss);
        tss.eflags = v86_pop32(tss);
      }
      else
      {
        if(tss.esp > 0x0ffa)
          iRetVal = 1; // Exit
        tss.eip    = v86_pop16(tss);
        tss.cs     = v86_pop16(tss);
        tss.eflags = (tss.eflags & 0xffff0000) | v86_pop16(tss);
      }
      break;
    }
    case 0x9c: // pushf
      if(prefix & PFX_OP32)
        v86_push32(tss, tss.eflags);
      else
        v86_push16(tss, tss.eflags);
      break;
    case 0x9d: // popf
      if(prefix & PFX_OP32)
      {
        if(tss.esp > 0x0ffc)
          iRetVal = -1; // Exit
        tss.eflags = v86_pop32(tss);
      }
      else
      {
        if(tss.esp > 0x0ffe)
          iRetVal = -1; // Exit
        tss.eflags = (tss.eflags & 0xffff0000) | v86_pop16(tss);
      }
      break;
    case 0xfa: // cli
      tss.eflags &= ~I386_IE_FLAG;
      break;
    case 0xfb: // sti
      tss.eflags |= I386_IE_FLAG;
      break;
    case 0xe4: // in al,imm8
      panic("v86 gpf: in al,imm8\n");
      break;
    case 0xe6: // out imm8,al
      panic("v86 gpf: out imm8,al\n");
      break;
    case 0xe5: // in eax,imm8
      panic("v86 gpf: in eax,imm8\n");
      break;
    case 0xe7: // out imm8,eax
      panic("v86 gpf: out imm8,eax\n");
      break;
    case 0x6c: // insb
      panic("v86 gpf: insb\n");
      break;
    case 0x6e: // outsb
      panic("v86 gpf: outsb\n");
      break;
    case 0xec: // in al,dx
      panic("v86 gpf: in al,dx\n");
      break;
    case 0xee: // out dx,al
      panic("v86 gpf: out dx,al\n");
      break;
    case 0x6d: // insw/insd
      panic("v86 gpf: insw/insd\n");
      break;
    case 0x6f: // outsw/outsd
      panic("v86 gpf: outsw/outsd\n");
      break;
    case 0xed: // in eax,dx
      panic("v86 gpf: in eax,dx\n");
      break;
    case 0xef: // out dx,eax
      panic("v86 gpf: out dx,eax\n");
      break;
    default:
      panic("v86 gpf, addr: 0x%x, inst: 0x%x\n", from_v86_addr(tss.cs, tss.eip), data);
  };

  return iRetVal;
}

// -----------------------------------------------------------------------------
void
init_gpf()
{
  // Create GPF Handler task
#ifdef CONFIG_MMU
  pGPFThread = new CThreadImpl(&pMainTask->aspace());
#else
  pGPFThread = new CThreadImpl;
#endif
  // Initialize GPF handler task
  pGPFThread->init((void *)_gpf);
  // Setup task-gate in IDT
  CDescriptorTable::createTaskGate(cIDT.desc_[0x0d], 0, pGPFThread->selTSS_);
  // Task will never be deleted
  //delete pGPFThread;
}

// -----------------------------------------------------------------------------
void
gpf()
{
  STaskStateSegment & tss = *pCurrentTSS;

  if(tss.eflags & (1<<17))
  {
    if(v86gpf_handler(tss) != 0)
    {
      // Modify our back-link, so we return to the task who called the v86 task
      pGPFThread->pTSS_->link = pCurrentTSS->link;
    }
  }
  else
    panic("GPF, addr: 0x%x\n", tss.eip);
}
