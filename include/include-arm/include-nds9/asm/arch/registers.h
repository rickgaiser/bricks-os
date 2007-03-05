#ifndef ASM_ARCH_REGISTERS_H
#define ASM_ARCH_REGISTERS_H


#include "inttypes.h"


typedef void (*fnptr)();
extern uint32_t __itcm_start;
extern uint32_t __ewram_end;
extern uint32_t __eheap_end;
extern uint32_t __dtcm_start;
extern uint32_t __dtcm_top;
extern uint32_t __irq_flags;  // DTCM + 0x3ff8
extern fnptr    __irq_vector; // DTCM + 0x3ffc


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
// "REG_POWCNT" bits
#define POWER_LCD             (1<<0)
#define POWER_2D_A            (1<<1)
#define POWER_MATRIX          (1<<2)
#define POWER_3D_CORE         (1<<3)
#define POWER_2D_B            (1<<9)
#define POWER_SWAP_LCDS       (1<<15)
#define POWER_ALL_2D          (POWER_LCD |POWER_2D_A |POWER_2D_B)
#define POWER_ALL             (POWER_ALL_2D | POWER_3D_CORE | POWER_MATRIX)
// "REG_DISPLAY_CR" bits
#define MODE_0_2D             0x10000
#define MODE_1_2D             0x10001
#define MODE_2_2D             0x10002
#define MODE_3_2D             0x10003
#define MODE_4_2D             0x10004
#define MODE_5_2D             0x10005
#define DISPLAY_BG0_ACTIVE    (1<<8)
#define DISPLAY_BG1_ACTIVE    (1<<9)
#define DISPLAY_BG2_ACTIVE    (1<<10)
#define DISPLAY_BG3_ACTIVE    (1<<11)
#define DISPLAY_SPR_ACTIVE    (1<<12)
#define DISPLAY_WIN0_ON       (1<<13)
#define DISPLAY_WIN1_ON       (1<<14)
#define DISPLAY_SPR_WIN_ON    (1<<15)
// "REG_VRAM_x_CR" bits
#define VRAM_ENABLE           (1<<7)
#define VRAM_A_LCD            0
#define VRAM_A_MAIN_BG        1
#define VRAM_A_MAIN_SPRITE    2
#define VRAM_A_TEXTURE        3
// "REG_BGx_CR" bits
#define BG_256_COLOR          (1<<7)
#define BG_16_COLOR           (0)
#define BG_TILE_BASE(base)    ((base)<<2)
#define BG_MAP_BASE(base)     ((base)<<8)
#define BG_BMP_BASE(base)     ((base)<<8)

// Memory Locations
#define BG_PALETTE            ( (uint16_t*)0x05000000)
#define SPRITE_PALETTE        ( (uint16_t*)0x05000200)
#define BG_PALETTE_SUB        ( (uint16_t*)0x05000400)
#define SPRITE_PALETTE_SUB    ( (uint16_t*)0x05000600)
#define CHAR_BASE_BLOCK(n)       (((n)*0x4000)+0x6000000)
#define CHAR_BASE_BLOCK_SUB(n)   (((n)*0x4000)+0x6200000)
#define SCREEN_BASE_BLOCK(n)     (((n)*0x0800)+0x6000000)
#define SCREEN_BASE_BLOCK_SUB(n) (((n)*0x0800)+0x6200000)

// Registers
#define REG_INTMAIN           (__irq_vector)
#define REG_DISPCNT           (*(vuint32_t*)0x04000000)
#define REG_DISPSTAT          (*(vuint16_t*)0x04000004)
#define REG_VCOUNT            (*(vuint16_t*)0x04000006)
#define REG_BGCTRL            ( (vuint16_t*)0x04000008)
#define REG_BG0_CR            (*(vuint16_t*)0x04000008)
#define REG_BG1_CR            (*(vuint16_t*)0x0400000A)
#define REG_BG2_CR            (*(vuint16_t*)0x0400000C)
#define REG_BG3_CR            (*(vuint16_t*)0x0400000E)
#define REG_BRIGHTNESS        (*(vuint16_t*)0x0400006C)
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
#define REG_IPC_SYNC          (*(vuint16_t*)0x04000180)
#define REG_IPC_FIFO_CR       (*(vuint16_t*)0x04000184)
#define REG_IPC_FIFO_TX       (*(vuint32_t*)0x04000188)
#define REG_IME               (*(vuint16_t*)0x04000208)
#define REG_IE                (*(vuint32_t*)0x04000210)
#define REG_IF                (*(vuint32_t*)0x04000214)
#define REG_VRAM_CR           (*(vuint32_t*)0x04000240)
#define REG_VRAM_A_CR         (*(vuint8_t *)0x04000240)
#define REG_VRAM_B_CR         (*(vuint8_t *)0x04000241)
#define REG_VRAM_C_CR         (*(vuint8_t *)0x04000242)
#define REG_VRAM_D_CR         (*(vuint8_t *)0x04000243)
#define REG_VRAM_E_CR         (*(vuint8_t *)0x04000244)
#define REG_VRAM_F_CR         (*(vuint8_t *)0x04000245)
#define REG_VRAM_G_CR         (*(vuint8_t *)0x04000246)
#define REG_WRAM_CR           (*(vuint8_t *)0x04000247)
#define REG_VRAM_H_CR         (*(vuint8_t *)0x04000248)
#define REG_VRAM_I_CR         (*(vuint8_t *)0x04000249)
#define REG_POWCNT            (*(vuint16_t*)0x04000304)
#define REG_SUB_DISPLAY_CR    (*(vuint32_t*)0x04001000)
#define REG_BGCTRL_SUB        ( (vuint16_t*)0x04001008)
#define REG_SUB_BG0_CR        (*(vuint16_t*)0x04001008)
#define REG_SUB_BG1_CR        (*(vuint16_t*)0x0400100A)
#define REG_SUB_BG2_CR        (*(vuint16_t*)0x0400100C)
#define REG_SUB_BG3_CR        (*(vuint16_t*)0x0400100E)
#define REG_SUB_BRIGHTNESS    (*(vuint16_t*)0x0400106C)
#define REG_IPC_FIFO_RX       (*(vuint32_t*)0x04100000)


#endif
