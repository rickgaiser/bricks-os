#ifndef ASM_IRQ_H
#define ASM_IRQ_H


#define MAX_INTERRUPTS 0x31


// -----------------------------------------------------------------------------
struct pt_regs
{
  // pushed by pusha
  unsigned long edi, esi, ebp, esp, ebx, edx, ecx, eax;
  // pushed separately
  unsigned long ss, ds, es, fs, gs;
  unsigned long iIntNumber, iErrorCode;
  // pushed by exception. Exception may also push err_code.
  // user_esp and user_ss are pushed only if a privilege change occurs.
  unsigned long eip, cs, eflags, user_esp, user_ss;
};


#endif
