#ifndef ASM_ARCH_REGISTERS_H
#define ASM_ARCH_REGISTERS_H


#include "inttypes.h"


typedef void (*fnptr)();
extern fnptr __irq_vector[];


// "REG_KEYS" bits
#define KEY_A                 (1<<0)
#define KEY_B                 (1<<1)
#define KEY_SELECT            (1<<2)
#define KEY_START             (1<<3)
#define KEY_RIGHT             (1<<4)
#define KEY_LEFT              (1<<5)
#define KEY_UP                (1<<6)
#define KEY_DOWN              (1<<7)
#define KEY_R                 (1<<8)
#define KEY_L                 (1<<9)
#define KEY_CNT_IRQ           (1<<14)
#define KEY_CNT_AND           (1<<15)
#define KEY_CNT_OR            (0)
// "REG_IPC_SYNC" bits
#define IPC_SYNC_IRQ_ENABLE   (1<<14)
#define IPC_SYNC_IRQ_REQUEST  (1<<13)
// "REG_IPC_FIFO_CR" bits
#define IPC_FIFO_SEND_EMPTY   (1<<0)
#define IPC_FIFO_SEND_FULL    (1<<1)
#define IPC_FIFO_SEND_IRQ     (1<<2)
#define IPC_FIFO_SEND_CLEAR   (1<<3)
#define IPC_FIFO_RECV_EMPTY   (1<<8)
#define IPC_FIFO_RECV_FULL    (1<<9)
#define IPC_FIFO_RECV_IRQ     (1<<10)
#define IPC_FIFO_ERROR        (1<<14)
#define IPC_FIFO_ENABLE       (1<<15)

// Registers
#define REG_INTMAIN           *(__irq_vector)
#define REG_IME               (*(vuint16_t*)0x04000208)
#define REG_IE                (*(vuint32_t*)0x04000210)
#define REG_IF                (*(vuint32_t*)0x04000214)
#define REG_TIME              ( (vuint16_t*)0x04000100)
#define REG_TM0D              (*(vuint16_t*)0x04000100)
#define REG_TM0CNT            (*(vuint16_t*)0x04000102)
#define REG_TM1D              (*(vuint16_t*)0x04000106)
#define REG_TM2D              (*(vuint16_t*)0x04000108)
#define REG_TM2CNT            (*(vuint16_t*)0x0400010A)
#define REG_TM3D              (*(vuint16_t*)0x0400010C)
#define REG_TM3CNT            (*(vuint16_t*)0x0400010E)
#define REG_KEYS              (*(vuint16_t*)0x04000130)
#define REG_KEYCNT            (*(vuint16_t*)0x04000132)
#define REG_KEYXY             (*(vuint16_t*)0x04000136)
#define REG_IPC_SYNC          (*(vuint16_t*)0x04000180)
#define REG_IPC_FIFO_CR       (*(vuint16_t*)0x04000184)
#define REG_IPC_FIFO_TX       (*(vuint32_t*)0x04000188)
#define REG_IPC_FIFO_RX       (*(vuint32_t*)0x04100000)


#endif
