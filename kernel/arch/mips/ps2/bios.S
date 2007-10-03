#include "regs.h"


.text
.p2align 3
.globl gs_set_imr
.globl gs_set_crtc
.globl ps2_flush_cache


# -----------
# From PS2SDK
# -----------
#ifdef USE_KMODE
#define SYSCALL(name, number) \
  .set push;            \
  .set noreorder;       \
  .text;                \
  .align 4;             \
  .globl name;          \
  .type name,@function; \
  .ent name,0;          \
name:                   \
  j  __syscall;         \
  li $3, number;        \
  nop;                  \
  .end  name;           \
  .size name,.-name;    \
  .set  pop;
#else
#define SYSCALL(name, number) \
  .set push;            \
  .set noreorder;       \
  .text;                \
  .align 4;             \
  .globl name;          \
  .type name,@function; \
  .ent name,0;          \
name:                   \
  li $3, number;        \
  syscall;              \
  jr $31;               \
  nop;                  \
  .end name;            \
  .size name,.-name;    \
  .set pop;
#endif


SYSCALL(resetEE,    0x01)
SYSCALL(setGsCrt,   0x02)
SYSCALL(flushCache, 0x64)
SYSCALL(getGsIMR,   0x70)
SYSCALL(setGsIMR,   0x71)
