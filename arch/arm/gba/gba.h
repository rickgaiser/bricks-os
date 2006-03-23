#ifndef GBA_H
#define GBA_H


typedef volatile unsigned short int  vu16;
typedef volatile unsigned int        vu32;

#define INT_VBLANK  0x0001 // 0
#define INT_HBLANK  0x0002 // 1
#define INT_VCOUNT  0x0004
#define INT_TIMER0  0x0008
#define INT_TIMER1  0x0010
#define INT_TIMER2  0x0020
#define INT_TIMER3  0x0040
#define INT_SERIAL  0x0080
#define INT_DMA0    0x0100
#define INT_DMA1    0x0200 // 9
#define INT_DMA2    0x0400 // 10
#define INT_DMA3    0x0800
#define INT_KEYPAD  0x1000
#define INT_CARD    0x2000 // 13

#define KEY_A       0x0001
#define KEY_B       0x0002
#define KEY_SELECT  0x0004
#define KEY_START   0x0008
#define KEY_RIGHT   0x0010
#define KEY_LEFT    0x0020
#define KEY_UP      0x0040
#define KEY_DOWN    0x0080
#define KEY_R       0x0100
#define KEY_L       0x0200
#define KEY_CNT_IRQ 0x4000
#define KEY_CNT_AND 0x8000
#define KEY_CNT_OR  0x0000

typedef void (*fnptr)();
#define REG_INTMAIN     (*(fnptr*)(0x03007ffc))
#define REG_DISPSTAT    (*(vu16*)0x4000004)		//???
#define REG_TM0D        (*(vu16*)0x4000100)		//Timer 0
#define REG_TM0CNT      (*(vu16*)0x4000102)		//Timer 0 Control
#define REG_TM1D        (*(vu32*)0x4000104)		//Timer 1
#define REG_TM1CNT  	(*(vu16*)0x4000106)		//Timer 1 Control
#define REG_TM2D        (*(vu16*)0x4000108)		//Timer 2
#define REG_TM2CNT      (*(vu16*)0x400010A)		//Timer 2 Control
#define REG_TM3D        (*(vu16*)0x400010C)		//Timer 3
#define REG_TM3CNT      (*(vu16*)0x400010E)		//Timer 3 Control

#define REG_IE          (*(vu16*)0x4000200)		//Interrupt Enable
#define REG_IF          (*(vu16*)0x4000202)		//Interrupt Flags
#define REG_IME         (*(vu16*)0x4000208)		//Interrupt Master Enable

#define REG_KEYS   (*((unsigned short*)0x04000130))
#define REG_KEYCNT (*((unsigned short*)0x04000132))


#endif
