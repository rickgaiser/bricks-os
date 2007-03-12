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


// CPU Frequencies
#ifdef GBA
#define CPU_FREQUENCY 16780000
#endif
#ifdef NDS
#define CPU_FREQUENCY 33513982
#endif

// "REG_DISPCNT" bits
#ifdef GBA
#define MODE_0                0
#define MODE_1                1
#define MODE_2                2
#define MODE_3                3
#define MODE_4                4
#define MODE_5                5
#endif // GBA
#ifdef NDS
#define MODE_0                0x10000
#define MODE_1                0x10001
#define MODE_2                0x10002
#define MODE_3                0x10003
#define MODE_4                0x10004
#define MODE_5                0x10005
#endif // NDS
#define BG0_ENABLE            (1<<8)
#define BG1_ENABLE            (1<<9)
#define BG2_ENABLE            (1<<10)
#define BG3_ENABLE            (1<<11)
#define SPRITE_ENABLE         (1<<12)
#define WIN0_ENABLE           (1<<13)
#define WIN1_ENABLE           (1<<14)
#define SPRITE_WIN_ENABLE     (1<<15)
// "REG_DISPSTAT" bits
#define IRQ_VBLANK_ENABLE     (1<<3)
#define IRQ_HBLANK_ENABLE     (1<<4)
#define IRQ_VCOUNT_ENABLE     (1<<5)
// "REG_BGxCNT" bits
#define BG_256_COLOR          (1<<7)
#define BG_16_COLOR           (0)
#define BG_TILE_BASE(base)    ((base)<<2)
#define BG_MAP_BASE(base)     ((base)<<8)
#define BG_BMP_BASE(base)     ((base)<<8)
#define BG_32x32              (0<<14)
#define BG_64x32              (1<<14)
#define BG_32x64              (2<<14)
#define BG_64x64              (3<<14)
#define BG_RS_16x16           (0<<14)
#define BG_RS_32x32           (1<<14)
#define BG_RS_64x64           (2<<14)
#define BG_RS_128x128         (3<<14)
#define BG_BMP8_128x128       (BG_RS_16x16 | BG_256_COLOR)
#define BG_BMP8_256x256       (BG_RS_32x32 | BG_256_COLOR)
#define BG_BMP8_512x256       (BG_RS_64x64 | BG_256_COLOR)
#define BG_BMP8_512x512       (BG_RS_128x128 | BG_256_COLOR)
#define BG_BMP8_1024x512      (1<<14)
#define BG_BMP8_512x1024      0
#define BG_BMP16_128x128      (BG_RS_16x16 | BG_256_COLOR | (1<<2))
#define BG_BMP16_256x256      (BG_RS_32x32 | BG_256_COLOR | (1<<2))
#define BG_BMP16_512x256      (BG_RS_64x64 | BG_256_COLOR | (1<<2))
#define BG_BMP16_512x512      (BG_RS_128x128 | BG_256_COLOR | (1<<2))
// "REG_DMAxCNT" bits
enum EDMASOURCEMODE
{
    DMA_SRC_INC =               (0<<23)
  , DMA_SRC_DEC =               (1<<23)
  , DMA_SRC_FIXED =             (2<<23)
};
enum EDMADESTINATIONMODE
{
    DMA_DST_INC =               (0<<21)
  , DMA_DST_DEC =               (1<<21)
  , DMA_DST_FIXED =             (2<<21)
  , DMA_DST_RELOAD =            (3<<21)
};
#define DMA_REPEAT            (1<<25)
#define DMA16                 (0<<26)
#define DMA32                 (1<<26)
#ifdef GBA
#define DMA_CARD              (1<<27)
enum EDMAMODE
{
    DMA_IMMEDIATE =             (0<<28)
  , DMA_VBLANK =                (1<<28)
  , DMA_HBLANK =                (2<<28)
  , DMA_SPECIAL =               (3<<28)
};
#endif // GBA
#ifdef NDS7
enum EDMAMODE
{
    DMA_IMMEDIATE =             (0<<28)
  , DMA_VBLANK =                (1<<28)
  , DMA_DSCARD =                (2<<28)
  , DMA_SPECIAL =               (3<<28)
};
#endif // NDS7
#ifdef NDS9
enum EDMAMODE
{
    DMA_IMMEDIATE =             (0<<27)
  , DMA_VBLANK =                (1<<27)
  , DMA_HBLANK =                (2<<27)
  , DMA_HSTART =                (3<<27)
  , DMA_MAIN_MEM_DISPLAY =      (4<<27)
  , DMA_DS_CARD =               (5<<27)
  , DMA_CARD =                  (6<<27)
  , DMA_GEOMETRY_FIFO =         (7<<27)
};
#endif // NDS9
#define DMA_IRQ               (1<<30)
#define DMA_ENABLE            (1<<31)
#define DMA_BUSY              (1<<31)
// "REG_TMxCNT" bits
#define TIMER_SPEED_0         0x0000  // CPU
#define TIMER_SPEED_1         0x0001  // CPU /   64
#define TIMER_SPEED_2         0x0002  // CPU /  256
#define TIMER_SPEED_3         0x0003  // CPU / 1024
#define TIMER_IRQ             0x0040
#define TIMER_ENABLE          0x0080
// "REG_KEYS" & "REG_KEYCNT" bits
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
#ifdef NDS7
// "REG_KEYSXY" bits
#define KEY_X                 (1<<0)
#define KEY_Y                 (1<<1)
#define KEY_TOUCH             (1<<6) // Touchscreen pendown.
#define KEY_LID               (1<<7) // Lid state.
#endif // NDS7
// "REG_KEYCNT" bits
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
// "REG_IE" & "REG_IF" bits
#define INT_VBLANK            (1<<0)
#define INT_HBLANK            (1<<1)
#define INT_VCOUNT            (1<<2)
#define INT_TIMER0            (1<<3)
#define INT_TIMER1            (1<<4)
#define INT_TIMER2            (1<<5)
#define INT_TIMER3            (1<<6)
#ifdef GBA
#define INT_SERIAL            (1<<7)
#endif // GBA
#define INT_DMA0              (1<<8)
#define INT_DMA1              (1<<9)
#define INT_DMA2              (1<<10)
#define INT_DMA3              (1<<11)
#define INT_KEYPAD            (1<<12)
#define INT_CARD              (1<<13)
#ifdef NDS
#define INT_IPC               (1<<16)
#define INT_FIFO_SEND_EMPTY   (1<<17)
#define INT_FIFO_RECV_NEMPTY  (1<<18)
#define INT_DSCARD_DATA       (1<<19)
#define INT_DSCARD            (1<<20)
#ifdef NDS9
#define INT_3D_CORE           (1<<21)
#endif // NDS9
#ifdef NDS7
#define INT_SCREEN_OPEN       (1<<22)
#define INT_SPI               (1<<23)
#define INT_WIFI              (1<<24)
#endif // NDS7
#endif // NDS
// "REG_VRAM_x_CR" bits
#define VRAM_ENABLE           (1<<7)
#define VRAM_TYPE_LCD         0
#define VRAM_TYPE_MAIN_BG     1
#define VRAM_TYPE_MAIN_SPRITE 2
#define VRAM_TYPE_TEXTURE     3
// "REG_POWCNT" bits
#ifdef NDS7
#define POWER_SOUND           (1<<0)
#define POWER_WIFI            (1<<1)
#endif // NDS7
#ifdef NDS9
#define POWER_LCD             (1<<0)
#define POWER_2D_BOTTOM       (1<<1)
#define POWER_3D_MATRIX       (1<<2)
#define POWER_3D_CORE         (1<<3)
#define POWER_2D_TOP          (1<<9)
#define POWER_2D_SWAP         (1<<15)
#endif // NDS9
#ifdef GBA
//
#define SIO_MODE_MASK         0x3000 // Communication mode
#define SIO_BAUD_RATE_MASK    0x0003 // Baud rate
#define SIO_ID_NO_MASK        0x0030 // Communication ID
//
#define SIO_MODE_SHIFT        12
#define SIO_BAUD_RATE_SHIFT   0
#define SIO_ID_NO_SHIFT       4
//
#define SIO_8BIT_MODE         0x0000 // Normal 8-bit communication mode
#define SIO_32BIT_MODE        0x1000 // Normal 32-bit communication mode
#define SIO_MULTI_MODE        0x2000 // Multi-play communication mode
#define SIO_UART_MODE         0x3000 // UART communication mode
//
#define SIO_SCK_OUT           0x0000 // Select external clock
#define SIO_SCK_IN            0x0001 // Select internal clock
#define SIO_IN_SCK_256K       0x0000 // Select internal clock 256KHz
#define SIO_IN_SCK_2M         0x0002 //                  Select 2MHz
#define SIO_ACK_RECV          0x0004 // Request transfer
#define SIO_ACK_SEND          0x0008 // Enable transfer
#define SIO_9600_BPS          0x0000 // Baud rate 9600 bps
#define SIO_38400_BPS         0x0001 //           68400 bps
#define SIO_57600_BPS         0x0002 //           57600 bps
#define SIO_115200_BPS        0x0003 //           115200 bps
#define SIO_MULTI_CONNECT     0x0004 // Multi-play communication connecting
#define SIO_MULTI_DISCONNECT  0x0000 //                          Disconnect
#define SIO_MULTI_PARENT      0x0008 // Multi-play communication  Connect master
#define SIO_MULTI_CHILD       0x0000 //                           Connect slave
#define SIO_MULTI_SI          0x0004 // Multi-play communication SI terminal
#define SIO_MULTI_SD          0x0008 //                          SD terminal
#define SIO_MULTI_BUSY        0x0080 // Multi-play communicating
#define SIO_CTS_ENABLE        0x0004 // Enable UART send enable signal
#define SIO_UART_7BIT         0x0000 // UART communication data length 7 bit
#define SIO_UART_8BIT         0x0080 //                                8 bit
#define SIO_ERROR             0x0040 // Detect error
#define SIO_START             0x0080 // Start transfer
#define SIO_ENABLE            0x0080 // Enable SIO
#define SIO_FIFO_ENABLE       0x0100 // Enable FIFO
#define SIO_PARITY_ENABLE     0x0200 // Enable parity
#define SIO_PARITY_EVEN       0x0000 // Even parity
#define SIO_PARITY_ODD        0x0008 // Odd parity
#define SIO_TRANS_ENABLE      0x0400 // Enable transmitter
#define SIO_TRANS_DATA_FULL   0x0010 // Transmitted data full
#define SIO_RECV_ENABLE       0x0800 // Enable receiver
#define SIO_RECV_DATA_EMPTY   0x0020 // No data received
#define SIO_IF_ENABLE         0x4000 // Enable interrupt request
#endif // GBA

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
#ifdef GBA
#define REG_INTMAIN           (*(fnptr*)(0x03007ffc))
#define REG_DISPCNT           (*(vuint16_t*)0x04000000) // See bits above
#endif // GBA
#ifdef NDS
#define REG_INTMAIN           (__irq_vector)
#define REG_DISPCNT           (*(vuint32_t*)0x04000000) // See bits above
#endif // NDS
#define REG_DISPSTAT          (*(vuint16_t*)0x04000004) // See bits above
#define REG_VCOUNT            (*(vuint16_t*)0x04000006)
#define REG_BG0CNT            (*(vuint16_t*)0x04000008) // See bits above
#define REG_BG1CNT            (*(vuint16_t*)0x0400000A) // See bits above
#define REG_BG2CNT            (*(vuint16_t*)0x0400000C) // See bits above
#define REG_BG3CNT            (*(vuint16_t*)0x0400000E) // See bits above
#define REG_BGCNT(n)          (*((&REG_BG0CNT)+n))
#define REG_BG0_X             (*(vuint16_t*)0x04000010)
#define REG_BG0_Y             (*(vuint16_t*)0x04000012)
#define REG_BG1_X             (*(vuint16_t*)0x04000014)
#define REG_BG1_Y             (*(vuint16_t*)0x04000016)
#define REG_BG2_X             (*(vuint16_t*)0x04000018)
#define REG_BG2_Y             (*(vuint16_t*)0x0400001A)
#define REG_BG3_X             (*(vuint16_t*)0x0400001C)
#define REG_BG3_Y             (*(vuint16_t*)0x0400001E)
#define REG_BG_X(n)           (*((&REG_BG0_X)+(n*2)))
#define REG_BG_Y(n)           (*((&REG_BG0_Y)+(n*2)))
#define REG_BG2_XDX           (*(vuint16_t*)0x04000020)
#define REG_BG2_XDY           (*(vuint16_t*)0x04000022)
#define REG_BG2_YDX           (*(vuint16_t*)0x04000024)
#define REG_BG2_YDY           (*(vuint16_t*)0x04000026)
#define REG_BG2_CX            (*(vuint32_t*)0x04000028)
#define REG_BG2_CY            (*(vuint32_t*)0x0400002C)
#define REG_BG3_XDX           (*(vuint16_t*)0x04000030)
#define REG_BG3_XDY           (*(vuint16_t*)0x04000032)
#define REG_BG3_YDX           (*(vuint16_t*)0x04000034)
#define REG_BG3_YDY           (*(vuint16_t*)0x04000036)
#define REG_BG3_CX            (*(vuint32_t*)0x04000038)
#define REG_BG3_CY            (*(vuint32_t*)0x0400003C)
#define REG_DMA0SAD           (*(vuint32_t*)0x040000b0) // DMA source addr
#define REG_DMA0DAD           (*(vuint32_t*)0x040000b4) // DMA destination addr
#define REG_DMA0CNT           (*(vuint32_t*)0x040000b8) // See bits above
#define REG_DMA1SAD           (*(vuint32_t*)0x040000bc) // DMA source addr
#define REG_DMA1DAD           (*(vuint32_t*)0x040000c0) // DMA destination addr
#define REG_DMA1CNT           (*(vuint32_t*)0x040000c4) // See bits above
#define REG_DMA2SAD           (*(vuint32_t*)0x040000c8) // DMA source addr
#define REG_DMA2DAD           (*(vuint32_t*)0x040000cc) // DMA destination addr
#define REG_DMA2CNT           (*(vuint32_t*)0x040000d0) // See bits above
#define REG_DMA3SAD           (*(vuint32_t*)0x040000d4) // DMA source addr
#define REG_DMA3DAD           (*(vuint32_t*)0x040000d8) // DMA destination addr
#define REG_DMA3CNT           (*(vuint32_t*)0x040000dc) // See bits above
#define REG_DMASAD(n)         (*((&REG_DMA0SAD)+(n*3)))
#define REG_DMADAD(n)         (*((&REG_DMA0DAD)+(n*3)))
#define REG_DMACNT(n)         (*((&REG_DMA0CNT)+(n*3)))
#ifdef NDS9
#define REG_DMA0FILL          (*(vuint32_t*)0x040000e0)
#define REG_DMA1FILL          (*(vuint32_t*)0x040000e4)
#define REG_DMA2FILL          (*(vuint32_t*)0x040000e8)
#define REG_DMA3FILL          (*(vuint32_t*)0x040000ec)
#define REG_DMAFILL(n)        (*((&REG_DMA0FILL)+n))
#endif // NDS9
#define REG_BRIGHTNESS        (*(vuint16_t*)0x0400006C)
#define REG_TM0D              (*(vuint16_t*)0x04000100)
#define REG_TM0CNT            (*(vuint16_t*)0x04000102) // See bits above
#define REG_TM1D              (*(vuint16_t*)0x04000104)
#define REG_TM1CNT            (*(vuint16_t*)0x04000106) // See bits above
#define REG_TM2D              (*(vuint16_t*)0x04000108)
#define REG_TM2CNT            (*(vuint16_t*)0x0400010A) // See bits above
#define REG_TM3D              (*(vuint16_t*)0x0400010C)
#define REG_TM3CNT            (*(vuint16_t*)0x0400010E) // See bits above
#define REG_TMCNT(n)          (*((&REG_TM0CNT)+(n*2)))
#define REG_TMD(n)            (*((&REG_TM0D)+(n*2)))
#ifdef GBA
#define REG_SIODATA32         (*(vuint32_t*)0x04000120) // Data Send Register (R/W)
#define REG_SIOMULTI0         (*(vuint16_t*)0x04000120) // SIO Multi-Player Data 0 (Parent) (R/W)
#define REG_SIOMULTI1         (*(vuint16_t*)0x04000122) // SIO Multi-Player Data 1 (1st child) (R/W)
#define REG_SIOMULTI2         (*(vuint16_t*)0x04000124) // SIO Multi-Player Data 2 (2nd child) (R/W)
#define REG_SIOMULTI3         (*(vuint16_t*)0x04000126) // SIO Multi-Player Data 3 (3rd child) (R/W)
#define REG_SIOCNT            (*(vuint16_t*)0x04000128) // Serial IO control
#define REG_SIODATA8          (*(vuint8_t *)0x0400012A) // Data Send Register (R/W)
#define REG_SIOMLT_SEND       (*(vuint16_t*)0x0400012A) //
#endif // GBA
#define REG_KEYS              (*(vuint16_t*)0x04000130) // See bits above
#define REG_KEYCNT            (*(vuint16_t*)0x04000132) // See bits above
#ifdef GBA
#define REG_RCNT              (*(vuint16_t*)0x04000134) //
#endif // GBA
#ifdef NDS7
#define REG_KEYSXY            (*(vuint16_t*)0x04000136) // See bits above
#endif // NDS7
#define REG_IPC_SYNC          (*(vuint16_t*)0x04000180) // See bits above
#define REG_IPC_FIFO_CR       (*(vuint16_t*)0x04000184) // See bits above
#define REG_IPC_FIFO_TX       (*(vuint32_t*)0x04000188)
#ifdef GBA
#define REG_IE                (*(vuint16_t*)0x04000200) // See bits above
#define REG_IF                (*(vuint16_t*)0x04000202) // See bits above
#endif // GBA
#define REG_IME               (*(vuint16_t*)0x04000208)
#ifdef NDS
#define REG_IE                (*(vuint32_t*)0x04000210) // See bits above
#define REG_IF                (*(vuint32_t*)0x04000214) // See bits above
#endif // NDS
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
#define REG_SUB_BG0_CR        (*(vuint16_t*)0x04001008)
#define REG_SUB_BG1_CR        (*(vuint16_t*)0x0400100A)
#define REG_SUB_BG2_CR        (*(vuint16_t*)0x0400100C)
#define REG_SUB_BG3_CR        (*(vuint16_t*)0x0400100E)
#define REG_SUB_BRIGHTNESS    (*(vuint16_t*)0x0400106C)
#define REG_IPC_FIFO_RX       (*(vuint32_t*)0x04100000)


#endif
