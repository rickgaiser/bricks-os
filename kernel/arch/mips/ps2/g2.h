//---------------------------------------------------------------------------
// File:	g2.h
// Author:	Tony Saveski, t_saveski@yahoo.com
// Notes:	Simple 'High Level' 2D Graphics Library
//---------------------------------------------------------------------------
#ifndef G2_H
#define G2_H

#include "inttypes.h"

#define SUCCESS		1
#define FAILURE		0

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	 PAL_256_256_32=0
	,PAL_320_256_32
	,PAL_384_256_32
	,PAL_512_256_32
	,PAL_640_256_32

	,NTSC_256_224_32
	,NTSC_320_224_32
	,NTSC_384_224_32
	,NTSC_512_224_32
	,NTSC_640_224_32
} g2_video_mode;

extern int g2_init(g2_video_mode mode);
extern void g2_end(void);

extern uint16_t g2_get_max_x(void);
extern uint16_t g2_get_max_y(void);

extern void g2_set_color(uint8_t r, uint8_t g, uint8_t b);
extern void g2_set_fill_color(uint8_t r, uint8_t g, uint8_t b);
extern void g2_get_color(uint8_t *r, uint8_t *g, uint8_t *b);
extern void g2_get_fill_color(uint8_t *r, uint8_t *g, uint8_t *b);

extern void g2_put_pixel(uint16_t x, uint16_t y);
extern void g2_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
extern void g2_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
extern void g2_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
extern void g2_put_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t *data);

extern void g2_set_viewport(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
extern void g2_get_viewport(uint16_t *x0, uint16_t *y0, uint16_t *x1, uint16_t *y1);

extern void g2_set_visible_frame(uint8_t frame);
extern void g2_set_active_frame(uint8_t frame);
extern uint8_t g2_get_visible_frame(void);
extern uint8_t g2_get_active_frame(void);

extern void g2_wait_vsync(void);
extern void g2_wait_hsync(void);

extern void g2_set_font(uint32_t *data, uint16_t w, uint16_t h, uint16_t *tc);
extern void g2_out_text(uint16_t x, uint16_t y, char *str);
extern void g2_set_font_spacing(uint16_t s);
extern uint16_t g2_get_font_spacing(void);
extern void g2_set_font_mag(uint16_t m);
extern uint16_t g2_get_font_mag(void);
extern void g2_set_font_color(uint8_t red, uint8_t green, uint8_t blue);

#ifdef __cplusplus
}
#endif

#endif // G2_H
