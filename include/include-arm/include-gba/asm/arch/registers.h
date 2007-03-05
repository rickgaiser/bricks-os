#ifndef ASM_ARCH_REGISTERS_H
#define ASM_ARCH_REGISTERS_H


#include "inttypes.h"


typedef void (*fnptr)();


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
// "REG_IF & REG_IE" bits
#define INT_VBLANK            (1<<0)
#define INT_HBLANK            (1<<1)
#define INT_VCOUNT            (1<<2)
#define INT_TIMER0            (1<<3)
#define INT_TIMER1            (1<<4)
#define INT_TIMER2            (1<<5)
#define INT_TIMER3            (1<<6)
#define INT_SERIAL            (1<<7)
#define INT_DMA0              (1<<8)
#define INT_DMA1              (1<<9)
#define INT_DMA2              (1<<10)
#define INT_DMA3              (1<<11)
#define INT_KEYPAD            (1<<12)
#define INT_CARD              (1<<13)
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

// Memory Locations
#define BG_PALETTE            ( ( uint16_t*)0x05000000)
#define SPRITE_PALETTE        ( ( uint16_t*)0x05000200)

// Registers
#define REG_INTMAIN           (*(fnptr*)(0x03007ffc))
#define REG_DISPCNT           (*(vuint16_t*)0x04000000) // Display Control (Mode)
#define REG_DISPSTAT          (*(vuint16_t*)0x04000004) // ???
#define REG_BG0CNT            (*(vuint16_t*)0x04000008) // Background 0
#define REG_BG0HOFS           (*(vuint16_t*)0x04000010) // Background 0 Horizontal Offset
#define REG_BG0VOFS           (*(vuint16_t*)0x04000012) // Background 0 Vertical Offset
#define REG_TM0D              (*(vuint16_t*)0x04000100) // Timer 0
#define REG_TM0CNT            (*(vuint16_t*)0x04000102) // Timer 0 Control
#define REG_TM1D              (*(vuint32_t*)0x04000104) // Timer 1
#define REG_TM1CNT            (*(vuint16_t*)0x04000106) // Timer 1 Control
#define REG_TM2D              (*(vuint16_t*)0x04000108) // Timer 2
#define REG_TM2CNT            (*(vuint16_t*)0x0400010A) // Timer 2 Control
#define REG_TM3D              (*(vuint16_t*)0x0400010C) // Timer 3
#define REG_TM3CNT            (*(vuint16_t*)0x0400010E) // Timer 3 Control
#define REG_SIODATA32         (*(vuint32_t*)0x04000120) // Data Send Register (R/W)
#define REG_SIOMULTI0         (*(vuint16_t*)0x04000120) // SIO Multi-Player Data 0 (Parent) (R/W)
#define REG_SIOMULTI1         (*(vuint16_t*)0x04000122) // SIO Multi-Player Data 1 (1st child) (R/W)
#define REG_SIOMULTI2         (*(vuint16_t*)0x04000124) // SIO Multi-Player Data 2 (2nd child) (R/W)
#define REG_SIOMULTI3         (*(vuint16_t*)0x04000126) // SIO Multi-Player Data 3 (3rd child) (R/W)
#define REG_SIOCNT            (*(vuint16_t*)0x04000128) // Serial IO control
#define REG_SIODATA8          (*(vuint8_t *)0x0400012A) // Data Send Register (R/W)
#define REG_SIOMLT_SEND       (*(vuint16_t*)0x0400012A) //
#define REG_KEYS              (*(vuint16_t*)0x04000130) // Keys
#define REG_KEYCNT            (*(vuint16_t*)0x04000132) // Key control
#define REG_RCNT              (*(vuint16_t*)0x04000134) //
#define REG_IE                (*(vuint16_t*)0x04000200) // Interrupt Enable
#define REG_IF                (*(vuint16_t*)0x04000202) // Interrupt Flags
#define REG_IME               (*(vuint16_t*)0x04000208) // Interrupt Master Enable


#endif
