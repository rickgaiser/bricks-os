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
// Pixel mode (Frame-, Z- and Textures)
#define GRAPH_PSM_32            0x00    // 32 bits per pixel.
#define GRAPH_PSM_24            0x01    // 24 bits per pixel.
#define GRAPH_PSM_16            0x02    // 16 bits per pixel.
#define GRAPH_PSM_16S           0x0A    // 16 bits per pixel.
// Pixel mode (Textures only)
#define GRAPH_PSM_8             0x13    // 8 bits per pixel, palettized.
#define GRAPH_PSM_4             0x14    // 4 bits per pixel, palettized.
#define GRAPH_PSM_8H            0x1B    // 8 bits per pixel, palettized.
#define GRAPH_PSM_4HH           0x24    // 4 bits per pixel, palettized.
#define GRAPH_PSM_4HL           0x2C    // 4 bits per pixel, palettized.


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
