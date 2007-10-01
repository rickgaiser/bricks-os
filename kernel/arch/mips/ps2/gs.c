//---------------------------------------------------------------------------
// File:	gs.c
// Author:	Tony Saveski, t_saveski@yahoo.com
//---------------------------------------------------------------------------
#include "ps2.h"
#include "gs.h"
#include "g2.h"
#include "gif.h"
#include "dma.h"

//---------------------------------------------------------------------------
#define MAX_TRANSFER	16384

//---------------------------------------------------------------------------
DECLARE_EXTERN_GS_PACKET(gs_dma_buf);

//---------------------------------------------------------------------------
void gs_load_texture(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t data_adr, uint32_t dest_adr, uint16_t dest_w)
{
uint32_t i;			// DMA buffer loop counter
uint32_t frac;		// flag for whether to run a fractional buffer or not
uint32_t current;		// number of pixels to transfer in current DMA
uint32_t qtotal;		// total number of qwords of data to transfer

	BEGIN_GS_PACKET(gs_dma_buf);
	GIF_TAG_AD(gs_dma_buf, 4, 1, 0, 0, 0);
	GIF_DATA_AD(gs_dma_buf, bitbltbuf,
		GS_BITBLTBUF(0, 0, 0,
			dest_adr/256,		// frame buffer address
			dest_w/64,			// frame buffer width
			0));
	GIF_DATA_AD(gs_dma_buf, trxpos,
		GS_TRXPOS(
			0,
			0,
			x,
			y,
			0));	// left to right/top to bottom
	GIF_DATA_AD(gs_dma_buf, trxreg, GS_TRXREG(w, h));
	GIF_DATA_AD(gs_dma_buf, trxdir, GS_TRXDIR(XDIR_EE_GS));
	SEND_GS_PACKET(gs_dma_buf);

	qtotal = w*h/4;					// total number of quadwords to transfer.
	current = qtotal % MAX_TRANSFER;// work out if a partial buffer transfer is needed.
	frac=1;							// assume yes.
	if(!current)					// if there is no need for partial buffer
	{
		current = MAX_TRANSFER;		// start with a full buffer
		frac=0;						// and don't do extra partial buffer first
	}
	for(i=0; i<(qtotal/MAX_TRANSFER)+frac; i++)
	{
		BEGIN_GS_PACKET(gs_dma_buf);
		GIF_TAG_IMG(gs_dma_buf, current);
		SEND_GS_PACKET(gs_dma_buf);

		SET_QWC(GIF_QWC, current);
		SET_MADR(GIF_MADR, data_adr, 0);
		SET_CHCR(GIF_CHCR, 1, 0, 0, 0, 0, 1, 0);
		DMA_WAIT(GIF_CHCR);

		data_adr += current*16;
		current = MAX_TRANSFER;		// after the first one, all are full buffers
	}

	// Access the TEXFLUSH register with anything to flush the texture
	BEGIN_GS_PACKET(gs_dma_buf);
	GIF_TAG_AD(gs_dma_buf, 1, 1, 0, 0, 0);
	GIF_DATA_AD(gs_dma_buf, texflush, 0x42);
	SEND_GS_PACKET(gs_dma_buf);
}

//---------------------------------------------------------------------------
uint16_t gs_texture_wh(uint16_t n)
{
uint16_t l=0;

	n--;
	while(n>0) n>>=1, l++;
	return(l);
}

//---------------------------------------------------------------------------
uint8_t gs_is_ntsc(void)
{
	if(*((char *)0x1FC80000 - 0xAE) != 'E') return(1);
	return(0);
}

//---------------------------------------------------------------------------
uint8_t gs_is_pal(void)
{
	if(*((char *)0x1FC80000 - 0xAE) == 'E') return(1);
	return(0);
}
