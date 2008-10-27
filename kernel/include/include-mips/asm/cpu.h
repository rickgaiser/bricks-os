#ifndef CPU_H
#define CPU_H


#include "mipsregisters.h"


// -----------------------------------------------------------------------------
// Clear EI bit in status register (0x00010000)
static inline void
local_irq_disable(void)
{
#ifdef PS2
  __asm__ volatile(
    "di\n\t" \
    "sync.p\n\t" \
  );
#endif
}

// -----------------------------------------------------------------------------
// Set EI bit in status register (0x00010000)
static inline void
local_irq_enable(void)
{
#ifdef PS2
  __asm__ volatile (
    "ei\n\t" \
  );
#endif
}

// -----------------------------------------------------------------------------
static inline unsigned long
local_save_flags(void)
{
#ifdef PS2
  unsigned long stat;

  __asm__ volatile ("mfc0 %0, $12" : "=r"(stat):);

  stat &= 0x00010000;

  return stat;
#else
  return 0;
#endif
}

// -----------------------------------------------------------------------------
static inline void
local_irq_restore(unsigned long flags)
{
#ifdef PS2
  if(flags & 0x00010000)
    local_irq_enable();
  else
    local_irq_disable();
#endif
}

// -----------------------------------------------------------------------------
static inline void
halt(void)
{
  while(1);
}


// -----------------------------------------------------------------------------
// MIPS Address space
#define KUSEG_START 0x00000000  // User segment: 2GiB, Mapped
#define KUSEG_SIZE  0x80000000
#define KSEG0_START 0x80000000  // Kernel segment 0, 512MiB, Unmapped, Cached
#define KSEG0_SIZE  0x20000000
#define KSEG1_START 0xa0000000  // Kernel segment 1, 512MiB, Unmapped, Uncached
#define KSEG1_SIZE  0x20000000
#define KSSEG_START 0xc0000000  // Supervisor segment, 512MiB, Mapped
#define KSSEG_SIZE  0x20000000
#define KSEG3_START 0xe0000000  // Kernel segment 3, 512MiB, Mapped
#define KSEG3_SIZE  0x20000000

// -----------------------------------------------------------------------------
// Address conversion macros
// NOTE: Only the first 512MiB of KUSEG can be converted from/to
#define ADDR_TO_KUSEG(a) ((__typeof__(a))(((uint32_t)(a) & 0x1fffffff) | KUSEG_START))
#define ADDR_TO_KSEG0(a) ((__typeof__(a))(((uint32_t)(a) & 0x1fffffff) | KSEG0_START))
#define ADDR_TO_KSEG1(a) ((__typeof__(a))(((uint32_t)(a) & 0x1fffffff) | KSEG1_START))
#define ADDR_TO_KSSEG(a) ((__typeof__(a))(((uint32_t)(a) & 0x1fffffff) | KSSEG_START))
#define ADDR_TO_KSEG3(a) ((__typeof__(a))(((uint32_t)(a) & 0x1fffffff) | KSEG3_START))

// -----------------------------------------------------------------------------
// Copied/Modified from linux/include/asm-mips/mipsregs.h
// -----------------------------------------------------------------------------
// The following macros are especially useful for __asm__
// inline assembler.
#ifndef __STR
#define __STR(x) #x
#endif
#ifndef STR
#define STR(x) __STR(x)
#endif

// -----------------------------------------------------------------------------
//  Configure language
#ifdef __ASSEMBLY__
#define _ULCAST_
#else
#define _ULCAST_ (unsigned long)
#endif

// -----------------------------------------------------------------------------
// Coprocessor 0 register names
#define CP0_INDEX                $0
#define CP0_RANDOM               $1
#define CP0_ENTRYLO0             $2
#define CP0_ENTRYLO1             $3
#define CP0_CONF                 $3
#define CP0_CONTEXT              $4
#define CP0_PAGEMASK             $5
#define CP0_WIRED                $6
#define CP0_INFO                 $7
#define CP0_BADVADDR             $8
#define CP0_COUNT                $9
#define CP0_ENTRYHI             $10
#define CP0_COMPARE             $11
#define CP0_STATUS              $12
#define CP0_CAUSE               $13
#define CP0_EPC                 $14
#define CP0_PRID                $15
#define CP0_CONFIG              $16
#define CP0_LLADDR              $17
#define CP0_WATCHLO             $18
#define CP0_WATCHHI             $19
#define CP0_XCONTEXT            $20
#define CP0_FRAMEMASK           $21
#define CP0_DIAGNOSTIC          $22
#define CP0_DEBUG               $23
#define CP0_DEPC                $24
#define CP0_PERFORMANCE         $25
#define CP0_ECC                 $26
#define CP0_CACHEERR            $27
#define CP0_TAGLO               $28
#define CP0_TAGHI               $29
#define CP0_ERROREPC            $30
#define CP0_DESAVE              $31

// Status register bits available in all MIPS CPUs.
#define ST0_IM                  0x0000ff00
#define  STATUSB_IP0            8
#define  STATUSF_IP0            (_ULCAST_(1) <<  8)
#define  STATUSB_IP1            9
#define  STATUSF_IP1            (_ULCAST_(1) <<  9)
#define  STATUSB_IP2            10
#define  STATUSF_IP2            (_ULCAST_(1) << 10)
#define  STATUSB_IP3            11
#define  STATUSF_IP3            (_ULCAST_(1) << 11)
#define  STATUSB_IP4            12
#define  STATUSF_IP4            (_ULCAST_(1) << 12)
#define  STATUSB_IP5            13
#define  STATUSF_IP5            (_ULCAST_(1) << 13)
#define  STATUSB_IP6            14
#define  STATUSF_IP6            (_ULCAST_(1) << 14)
#define  STATUSB_IP7            15
#define  STATUSF_IP7            (_ULCAST_(1) << 15)
#define  STATUSB_IP8            0
#define  STATUSF_IP8            (_ULCAST_(1) <<  0)
#define  STATUSB_IP9            1
#define  STATUSF_IP9            (_ULCAST_(1) <<  1)
#define  STATUSB_IP10           2
#define  STATUSF_IP10           (_ULCAST_(1) <<  2)
#define  STATUSB_IP11           3
#define  STATUSF_IP11           (_ULCAST_(1) <<  3)
#define  STATUSB_IP12           4
#define  STATUSF_IP12           (_ULCAST_(1) <<  4)
#define  STATUSB_IP13           5
#define  STATUSF_IP13           (_ULCAST_(1) <<  5)
#define  STATUSB_IP14           6
#define  STATUSF_IP14           (_ULCAST_(1) <<  6)
#define  STATUSB_IP15           7
#define  STATUSF_IP15           (_ULCAST_(1) <<  7)
#define ST0_CH                  0x00040000
#define ST0_SR                  0x00100000
#define ST0_TS                  0x00200000
#define ST0_BEV                 0x00400000
#define ST0_RE                  0x02000000
#define ST0_FR                  0x04000000
#define ST0_CU                  0xf0000000
#define ST0_CU0                 0x10000000
#define ST0_CU1                 0x20000000
#define ST0_CU2                 0x40000000
#define ST0_CU3                 0x80000000
#define ST0_XX                  0x80000000 // MIPS IV naming

// Bitfields and bit numbers in the coprocessor 0 cause register.
// Refer to your MIPS R4xx0 manual, chapter 5 for explanation.
#define  CAUSEB_EXCCODE         2
#define  CAUSEF_EXCCODE         (_ULCAST_(31)  <<  2)
#define  CAUSEB_IP              8
#define  CAUSEF_IP              (_ULCAST_(255) <<  8)
#define  CAUSEB_IP0             8
#define  CAUSEF_IP0             (_ULCAST_(1)   <<  8)
#define  CAUSEB_IP1             9
#define  CAUSEF_IP1             (_ULCAST_(1)   <<  9)
#define  CAUSEB_IP2             10
#define  CAUSEF_IP2             (_ULCAST_(1)   << 10)
#define  CAUSEB_IP3             11
#define  CAUSEF_IP3             (_ULCAST_(1)   << 11)
#define  CAUSEB_IP4             12
#define  CAUSEF_IP4             (_ULCAST_(1)   << 12)
#define  CAUSEB_IP5             13
#define  CAUSEF_IP5             (_ULCAST_(1)   << 13)
#define  CAUSEB_IP6             14
#define  CAUSEF_IP6             (_ULCAST_(1)   << 14)
#define  CAUSEB_IP7             15
#define  CAUSEF_IP7             (_ULCAST_(1)   << 15)
#define  CAUSEB_IV              23
#define  CAUSEF_IV              (_ULCAST_(1)   << 23)
#define  CAUSEB_CE              28
#define  CAUSEF_CE              (_ULCAST_(3)   << 28)
#define  CAUSEB_BD              31
#define  CAUSEF_BD              (_ULCAST_(1)   << 31)


#define __read_32bit_c0_register(source, sel)           \
({ int __res;                                           \
  if (sel == 0)                                         \
    __asm__ __volatile__(                               \
      "mfc0\t%0, " #source "\n\t"                       \
      : "=r" (__res));                                  \
  else                                                  \
    __asm__ __volatile__(                               \
      ".set\tmips32\n\t"                                \
      "mfc0\t%0, " #source ", " #sel "\n\t"             \
      ".set\tmips0\n\t"                                 \
      : "=r" (__res));                                  \
  __res;                                                \
})

#define __read_64bit_c0_register(source, sel)           \
({ unsigned long long __res;                            \
  if (sizeof(unsigned long) == 4)                       \
    __res = __read_64bit_c0_split(source, sel);         \
  else if (sel == 0)                                    \
    __asm__ __volatile__(                               \
      ".set\tmips3\n\t"                                 \
      "dmfc0\t%0, " #source "\n\t"                      \
      ".set\tmips0"                                     \
      : "=r" (__res));                                  \
  else                                                  \
    __asm__ __volatile__(                               \
      ".set\tmips64\n\t"                                \
      "dmfc0\t%0, " #source ", " #sel "\n\t"            \
      ".set\tmips0"                                     \
      : "=r" (__res));                                  \
  __res;                                                \
})

#define __write_32bit_c0_register(register, sel, value) \
do {                                                    \
  if (sel == 0)                                         \
    __asm__ __volatile__(                               \
      "mtc0\t%z0, " #register "\n\t"                    \
      : : "Jr" ((unsigned int)(value)));                \
  else                                                  \
    __asm__ __volatile__(                               \
      ".set\tmips32\n\t"                                \
      "mtc0\t%z0, " #register ", " #sel "\n\t"          \
      ".set\tmips0"                                     \
      : : "Jr" ((unsigned int)(value)));                \
} while (0)

#define __write_64bit_c0_register(register, sel, value) \
do {                                                    \
  if (sizeof(unsigned long) == 4)                       \
    __write_64bit_c0_split(register, sel, value);       \
  else if (sel == 0)                                    \
    __asm__ __volatile__(                               \
      ".set\tmips3\n\t"                                 \
      "dmtc0\t%z0, " #register "\n\t"                   \
      ".set\tmips0"                                     \
      : : "Jr" (value));                                \
  else                                                  \
    __asm__ __volatile__(                               \
      ".set\tmips64\n\t"                                \
      "dmtc0\t%z0, " #register ", " #sel "\n\t"         \
      ".set\tmips0"                                     \
      : : "Jr" (value));                                \
} while (0)

#define __read_ulong_c0_register(reg, sel)              \
  ((sizeof(unsigned long) == 4) ?                       \
  (unsigned long) __read_32bit_c0_register(reg, sel) :  \
  (unsigned long) __read_64bit_c0_register(reg, sel))

#define __write_ulong_c0_register(reg, sel, val)        \
do {                                                    \
  if (sizeof(unsigned long) == 4)                       \
    __write_32bit_c0_register(reg, sel, val);           \
  else                                                  \
    __write_64bit_c0_register(reg, sel, val);           \
} while (0)


#define read_c0_index()         __read_32bit_c0_register($0, 0)
#define write_c0_index(val)     __write_32bit_c0_register($0, 0, val)

#define read_c0_entrylo0()      __read_ulong_c0_register($2, 0)
#define write_c0_entrylo0(val)  __write_ulong_c0_register($2, 0, val)

#define read_c0_entrylo1()      __read_ulong_c0_register($3, 0)
#define write_c0_entrylo1(val)  __write_ulong_c0_register($3, 0, val)

#define read_c0_conf()          __read_32bit_c0_register($3, 0)
#define write_c0_conf(val)      __write_32bit_c0_register($3, 0, val)

#define read_c0_context()       __read_ulong_c0_register($4, 0)
#define write_c0_context(val)   __write_ulong_c0_register($4, 0, val)

#define read_c0_pagemask()      __read_32bit_c0_register($5, 0)
#define write_c0_pagemask(val)  __write_32bit_c0_register($5, 0, val)

#define read_c0_wired()         __read_32bit_c0_register($6, 0)
#define write_c0_wired(val)     __write_32bit_c0_register($6, 0, val)

#define read_c0_info()          __read_32bit_c0_register($7, 0)

#define read_c0_cache()         __read_32bit_c0_register($7, 0)
#define write_c0_cache(val)     __write_32bit_c0_register($7, 0, val)

#define read_c0_badvaddr()      __read_ulong_c0_register($8, 0)
#define write_c0_badvaddr(val)  __write_ulong_c0_register($8, 0, val)

#define read_c0_count()         __read_32bit_c0_register($9, 0)
#define write_c0_count(val)     __write_32bit_c0_register($9, 0, val)

#define read_c0_entryhi()       __read_ulong_c0_register($10, 0)
#define write_c0_entryhi(val)   __write_ulong_c0_register($10, 0, val)

#define read_c0_compare()       __read_32bit_c0_register($11, 0)
#define write_c0_compare(val)   __write_32bit_c0_register($11, 0, val)

#define read_c0_status()        __read_32bit_c0_register($12, 0)
#define write_c0_status(val)    __write_32bit_c0_register($12, 0, val)

#define read_c0_cause()         __read_32bit_c0_register($13, 0)
#define write_c0_cause(val)     __write_32bit_c0_register($13, 0, val)

#define read_c0_epc()           __read_ulong_c0_register($14, 0)
#define write_c0_epc(val)       __write_ulong_c0_register($14, 0, val)

#define read_c0_prid()          __read_32bit_c0_register($15, 0)

#define read_c0_config()        __read_32bit_c0_register($16, 0)
#define read_c0_config1()       __read_32bit_c0_register($16, 1)
#define read_c0_config2()       __read_32bit_c0_register($16, 2)
#define read_c0_config3()       __read_32bit_c0_register($16, 3)
#define read_c0_config4()       __read_32bit_c0_register($16, 4)
#define read_c0_config5()       __read_32bit_c0_register($16, 5)
#define read_c0_config6()       __read_32bit_c0_register($16, 6)
#define read_c0_config7()       __read_32bit_c0_register($16, 7)
#define write_c0_config(val)    __write_32bit_c0_register($16, 0, val)
#define write_c0_config1(val)   __write_32bit_c0_register($16, 1, val)
#define write_c0_config2(val)   __write_32bit_c0_register($16, 2, val)
#define write_c0_config3(val)   __write_32bit_c0_register($16, 3, val)
#define write_c0_config4(val)   __write_32bit_c0_register($16, 4, val)
#define write_c0_config5(val)   __write_32bit_c0_register($16, 5, val)
#define write_c0_config6(val)   __write_32bit_c0_register($16, 6, val)
#define write_c0_config7(val)   __write_32bit_c0_register($16, 7, val)

// The WatchLo register.  There may be upto 8 of them.
#define read_c0_watchlo0()      __read_ulong_c0_register($18, 0)
#define read_c0_watchlo1()      __read_ulong_c0_register($18, 1)
#define read_c0_watchlo2()      __read_ulong_c0_register($18, 2)
#define read_c0_watchlo3()      __read_ulong_c0_register($18, 3)
#define read_c0_watchlo4()      __read_ulong_c0_register($18, 4)
#define read_c0_watchlo5()      __read_ulong_c0_register($18, 5)
#define read_c0_watchlo6()      __read_ulong_c0_register($18, 6)
#define read_c0_watchlo7()      __read_ulong_c0_register($18, 7)
#define write_c0_watchlo0(val)  __write_ulong_c0_register($18, 0, val)
#define write_c0_watchlo1(val)  __write_ulong_c0_register($18, 1, val)
#define write_c0_watchlo2(val)  __write_ulong_c0_register($18, 2, val)
#define write_c0_watchlo3(val)  __write_ulong_c0_register($18, 3, val)
#define write_c0_watchlo4(val)  __write_ulong_c0_register($18, 4, val)
#define write_c0_watchlo5(val)  __write_ulong_c0_register($18, 5, val)
#define write_c0_watchlo6(val)  __write_ulong_c0_register($18, 6, val)
#define write_c0_watchlo7(val)  __write_ulong_c0_register($18, 7, val)

// The WatchHi register.  There may be upto 8 of them.
#define read_c0_watchhi0()      __read_32bit_c0_register($19, 0)
#define read_c0_watchhi1()      __read_32bit_c0_register($19, 1)
#define read_c0_watchhi2()      __read_32bit_c0_register($19, 2)
#define read_c0_watchhi3()      __read_32bit_c0_register($19, 3)
#define read_c0_watchhi4()      __read_32bit_c0_register($19, 4)
#define read_c0_watchhi5()      __read_32bit_c0_register($19, 5)
#define read_c0_watchhi6()      __read_32bit_c0_register($19, 6)
#define read_c0_watchhi7()      __read_32bit_c0_register($19, 7)

#define write_c0_watchhi0(val)  __write_32bit_c0_register($19, 0, val)
#define write_c0_watchhi1(val)  __write_32bit_c0_register($19, 1, val)
#define write_c0_watchhi2(val)  __write_32bit_c0_register($19, 2, val)
#define write_c0_watchhi3(val)  __write_32bit_c0_register($19, 3, val)
#define write_c0_watchhi4(val)  __write_32bit_c0_register($19, 4, val)
#define write_c0_watchhi5(val)  __write_32bit_c0_register($19, 5, val)
#define write_c0_watchhi6(val)  __write_32bit_c0_register($19, 6, val)
#define write_c0_watchhi7(val)  __write_32bit_c0_register($19, 7, val)

#define read_c0_xcontext()      __read_ulong_c0_register($20, 0)
#define write_c0_xcontext(val)  __write_ulong_c0_register($20, 0, val)

#define read_c0_intcontrol()    __read_32bit_c0_ctrl_register($20)
#define write_c0_intcontrol(val) __write_32bit_c0_ctrl_register($20, val)

#define read_c0_framemask()     __read_32bit_c0_register($21, 0)
#define write_c0_framemask(val) __write_32bit_c0_register($21, 0, val)

// RM9000 PerfControl performance counter control register
#define read_c0_perfcontrol()   __read_32bit_c0_register($22, 0)
#define write_c0_perfcontrol(val) __write_32bit_c0_register($22, 0, val)

#define read_c0_diag()          __read_32bit_c0_register($22, 0)
#define write_c0_diag(val)      __write_32bit_c0_register($22, 0, val)

#define read_c0_diag1()         __read_32bit_c0_register($22, 1)
#define write_c0_diag1(val)     __write_32bit_c0_register($22, 1, val)

#define read_c0_diag2()         __read_32bit_c0_register($22, 2)
#define write_c0_diag2(val)     __write_32bit_c0_register($22, 2, val)

#define read_c0_diag3()         __read_32bit_c0_register($22, 3)
#define write_c0_diag3(val)     __write_32bit_c0_register($22, 3, val)

#define read_c0_diag4()         __read_32bit_c0_register($22, 4)
#define write_c0_diag4(val)     __write_32bit_c0_register($22, 4, val)

#define read_c0_diag5()         __read_32bit_c0_register($22, 5)
#define write_c0_diag5(val)     __write_32bit_c0_register($22, 5, val)

#define read_c0_debug()         __read_32bit_c0_register($23, 0)
#define write_c0_debug(val)     __write_32bit_c0_register($23, 0, val)

#define read_c0_depc()          __read_ulong_c0_register($24, 0)
#define write_c0_depc(val)      __write_ulong_c0_register($24, 0, val)

// MIPS32 / MIPS64 performance counters
#define read_c0_perfctrl0()     __read_32bit_c0_register($25, 0)
#define write_c0_perfctrl0(val) __write_32bit_c0_register($25, 0, val)
#define read_c0_perfcntr0()     __read_32bit_c0_register($25, 1)
#define write_c0_perfcntr0(val) __write_32bit_c0_register($25, 1, val)
#define read_c0_perfctrl1()     __read_32bit_c0_register($25, 2)
#define write_c0_perfctrl1(val) __write_32bit_c0_register($25, 2, val)
#define read_c0_perfcntr1()     __read_32bit_c0_register($25, 3)
#define write_c0_perfcntr1(val) __write_32bit_c0_register($25, 3, val)
#define read_c0_perfctrl2()     __read_32bit_c0_register($25, 4)
#define write_c0_perfctrl2(val) __write_32bit_c0_register($25, 4, val)
#define read_c0_perfcntr2()     __read_32bit_c0_register($25, 5)
#define write_c0_perfcntr2(val) __write_32bit_c0_register($25, 5, val)
#define read_c0_perfctrl3()     __read_32bit_c0_register($25, 6)
#define write_c0_perfctrl3(val) __write_32bit_c0_register($25, 6, val)
#define read_c0_perfcntr3()     __read_32bit_c0_register($25, 7)
#define write_c0_perfcntr3(val) __write_32bit_c0_register($25, 7, val)

// RM9000 PerfCount performance counter register
#define read_c0_perfcount()     __read_64bit_c0_register($25, 0)
#define write_c0_perfcount(val) __write_64bit_c0_register($25, 0, val)

#define read_c0_ecc()           __read_32bit_c0_register($26, 0)
#define write_c0_ecc(val)       __write_32bit_c0_register($26, 0, val)

#define read_c0_derraddr0()     __read_ulong_c0_register($26, 1)
#define write_c0_derraddr0(val) __write_ulong_c0_register($26, 1, val)

#define read_c0_cacheerr()      __read_32bit_c0_register($27, 0)

#define read_c0_derraddr1()     __read_ulong_c0_register($27, 1)
#define write_c0_derraddr1(val) __write_ulong_c0_register($27, 1, val)

#define read_c0_taglo()         __read_32bit_c0_register($28, 0)
#define write_c0_taglo(val)     __write_32bit_c0_register($28, 0, val)

#define read_c0_dtaglo()        __read_32bit_c0_register($28, 2)
#define write_c0_dtaglo(val)    __write_32bit_c0_register($28, 2, val)

#define read_c0_taghi()         __read_32bit_c0_register($29, 0)
#define write_c0_taghi(val)     __write_32bit_c0_register($29, 0, val)

#define read_c0_errorepc()      __read_ulong_c0_register($30, 0)
#define write_c0_errorepc(val)  __write_ulong_c0_register($30, 0, val)

// MIPSR2
#define read_c0_hwrena()        __read_32bit_c0_register($7,0)
#define write_c0_hwrena(val)    __write_32bit_c0_register($7, 0, val)

#define read_c0_intctl()        __read_32bit_c0_register($12, 1)
#define write_c0_intctl(val)    __write_32bit_c0_register($12, 1, val)

#define read_c0_srsctl()        __read_32bit_c0_register($12, 2)
#define write_c0_srsctl(val)    __write_32bit_c0_register($12, 2, val)

#define read_c0_srsmap()        __read_32bit_c0_register($12, 3)
#define write_c0_srsmap(val)    __write_32bit_c0_register($12, 3, val)

#define read_c0_ebase()         __read_32bit_c0_register($15,1)
#define write_c0_ebase(val)     __write_32bit_c0_register($15, 1, val)


#define __BUILD_SET_C0(name)                                    \
static inline unsigned int                                      \
set_c0_##name(unsigned int set)                                 \
{                                                               \
        unsigned int res;                                       \
                                                                \
        res = read_c0_##name();                                 \
        res |= set;                                             \
        write_c0_##name(res);                                   \
                                                                \
        return res;                                             \
}                                                               \
                                                                \
static inline unsigned int                                      \
clear_c0_##name(unsigned int clear)                             \
{                                                               \
        unsigned int res;                                       \
                                                                \
        res = read_c0_##name();                                 \
        res &= ~clear;                                          \
        write_c0_##name(res);                                   \
                                                                \
        return res;                                             \
}                                                               \
                                                                \
static inline unsigned int                                      \
change_c0_##name(unsigned int change, unsigned int val)         \
{                                                               \
        unsigned int res;                                       \
                                                                \
        res = read_c0_##name();                                 \
        res &= ~change;                                         \
        res |= (val & change);                                  \
        write_c0_##name(res);                                   \
                                                                \
        return res;                                             \
}

__BUILD_SET_C0(status)
__BUILD_SET_C0(cause)
__BUILD_SET_C0(config)
//__BUILD_SET_C0(intcontrol)
__BUILD_SET_C0(intctl)
__BUILD_SET_C0(srsmap)


#endif
