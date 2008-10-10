#ifndef PS2_GS_H
#define PS2_GS_H


#include "inttypes.h"
#include "asm/arch/registers.h"


// Interlacing
#define NON_INTERLACED          0
#define INTERLACED              1
// Field mode (only if INTERLACED)
#define FIELD                   0       // Scan every other line
#define FRAME                   1       // Scan every line

// Pixel Storage Modes
//   NOTE, how to fully fill the video memory:
//     GRAPH_PSM_24 + GRAPH_PSM_8H = 32bit
//     GRAPH_PSM_24 + GRAPH_PSM_4HH + GRAPH_PSM_4HL = 32bit
//   NOTE2, alignment:
//     framebuffer:  8KiB
//     z-buffer:     8KiB
//     textures:   256Bytes
//     clut:       256Bytes
//   NOTE3, 16bit frame- and z-buffers
//     Always use GRAPH_PSM_16S when only one is 16bit
//     When both frame- and z-buffer are 16bit, use GRAPH_PSM_16
// Frame-, Z- and Textures
#define GRAPH_PSM_32            0x00    // 32 bits per pixel.
#define GRAPH_PSM_24            0x01    // 24 bits per pixel. Stored in bits 0..23 of a 32 bit value.
#define GRAPH_PSM_16            0x02    // 16 bits per pixel.
#define GRAPH_PSM_16S           0x0A    // 16 bits per pixel.
// Textures only
#define GRAPH_PSM_8             0x13    // 8 bits per pixel, palettized.
#define GRAPH_PSM_4             0x14    // 4 bits per pixel, palettized.
#define GRAPH_PSM_8H            0x1B    // 8 bits per pixel, palettized. Stored int bits 24..31 of a 32 bit value
#define GRAPH_PSM_4HH           0x24    // 4 bits per pixel, palettized. Stored int bits 28..31 of a 32 bit value
#define GRAPH_PSM_4HL           0x2C    // 4 bits per pixel, palettized. Stored int bits 24..27 of a 32 bit value

#define GS_X_BASE               1024
#define GS_Y_BASE               1024
#define GS_MEMORY_SIZE          (4*1024*1024)

#define DRAW_FIELD_EVEN()       (GS_REG_CSR & (1 << 13))
#define DISPLAY_FIELD_EVEN()    (!DRAW_FIELD_EVEN)
#define WAIT_VSYNC()            {REG_GS_CSR = REG_GS_CSR & 8; while(!(REG_GS_CSR & 8));}
#define IS_PAL()                (*((char *)0x1fc80000 - 0xae) == 'E')
#define IS_NTSC()               (!IS_PAL())


#ifdef __cplusplus
extern "C" {
#endif


void       ee_to_gsBitBlt(uint32_t daddr, int dw, int dpsm, int dx, int dy, int w, int h, uint32_t source);
void       gs_to_gsBitBlt(uint32_t daddr, int dw, int dpsm, int dx, int dy, int w, int h, uint32_t source, int sw, int spsm, int sx, int sy);
uint8_t    getBitNr(uint32_t value);


#ifdef __cplusplus
}
#endif


#endif // PS2_GS_H
