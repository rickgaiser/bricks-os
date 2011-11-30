/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2010 Maximus32 <Maximus32@bricks-os.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA
 */


#ifndef SB16_H
#define SB16_H


#include "stdint.h"
#include "i8237.h"
#include "kernel/interrupt.h"
#include "kernel/pthread_k.h"


#define SB_TIMEOUT              32000           /* timeout count */

/* IRQ, base address and DMA channels */
#define SB_IRQ                  5
#define SB_BASE_ADDR            0x220           // 0x210, 0x220, 0x230, 0x240, 0x250, 0x260, 0x280
#define SB_DMA_8                1               // 0, 1, 3
#define SB_DMA_16               5               // 5, 6, 7

/* Some defaults for the DSP */
#define DEFAULT_SPEED           22050           /* Sample rate */
#define DEFAULT_BITS            8               /* Nr. of bits */
#define DEFAULT_SIGN            0               /* 0 = unsigned, 1 = signed */
#define DEFAULT_STEREO          0               /* 0 = mono, 1 = stereo */

/* DMA modes */
#define DMA16_AUTO_PLAY         0x58 + (SB_DMA_16 & 3)
#define DMA16_AUTO_REC          0x54 + (SB_DMA_16 & 3)
#define DMA8_AUTO_PLAY          0x58 + SB_DMA_8
#define DMA8_AUTO_REC           0x54 + SB_DMA_8

/* IO ports for soundblaster */
#define DSP_RESET               0x6 + SB_BASE_ADDR
#define DSP_READ                0xA + SB_BASE_ADDR
#define DSP_WRITE               0xC + SB_BASE_ADDR
#define DSP_COMMAND             0xC + SB_BASE_ADDR
#define DSP_STATUS              0xC + SB_BASE_ADDR
#define DSP_DATA_AVL            0xE + SB_BASE_ADDR
#define DSP_DATA16_AVL          0xF + SB_BASE_ADDR
#define MIXER_REG               0x4 + SB_BASE_ADDR
#define MIXER_DATA              0x5 + SB_BASE_ADDR
#define OPL3_LEFT               0x0 + SB_BASE_ADDR
#define OPL3_RIGHT              0x2 + SB_BASE_ADDR
#define OPL3_BOTH               0x8 + SB_BASE_ADDR

/* DSP Commands */
#define DSP_INPUT_RATE          0x42  /* set input sample rate */
#define DSP_OUTPUT_RATE         0x41  /* set output sample rate */
#define DSP_CMD_SPKON           0xD1  /* set speaker on */
#define DSP_CMD_SPKOFF          0xD3  /* set speaker off */
#define DSP_CMD_DMA8HALT        0xD0  /* halt DMA 8-bit operation */
#define DSP_CMD_DMA8CONT        0xD4  /* continue DMA 8-bit operation */
#define DSP_CMD_DMA16HALT       0xD5  /* halt DMA 16-bit operation */
#define DSP_CMD_DMA16CONT       0xD6  /* continue DMA 16-bit operation */
#define DSP_GET_VERSION         0xE1  /* get version number of DSP */
#define DSP_CMD_8BITAUTO_IN     0xCE  /* 8 bit auto-initialized input */
#define DSP_CMD_8BITAUTO_OUT    0xC6  /* 8 bit auto-initialized output */
#define DSP_CMD_16BITAUTO_IN    0xBE  /* 16 bit auto-initialized input */
#define DSP_CMD_16BITAUTO_OUT   0xB6  /* 16 bit auto-initialized output */
#define DSP_CMD_IRQREQ8         0xF2  /* Interrupt request 8 bit        */
#define DSP_CMD_IRQREQ16        0xF3  /* Interrupt request 16 bit        */

/* DSP Modes */
#define DSP_MODE_MONO_US        0x00  /* Mono unsigned */
#define DSP_MODE_MONO_S         0x10  /* Mono signed */
#define DSP_MODE_STEREO_US      0x20  /* Stereo unsigned */
#define DSP_MODE_STEREO_S       0x30  /* Stereo signed */

/* MIXER commands */
#define MIXER_RESET             0x00  /* Reset */
#define MIXER_DAC_LEVEL         0x04  /* Used for detection only */
#define MIXER_MASTER_LEFT       0x30  /* Master volume left */
#define MIXER_MASTER_RIGHT      0x31  /* Master volume right */
#define MIXER_DAC_LEFT          0x32  /* Dac level left */
#define MIXER_DAC_RIGHT         0x33  /* Dac level right */
#define MIXER_FM_LEFT           0x34  /* Fm level left */
#define MIXER_FM_RIGHT          0x35  /* Fm level right */
#define MIXER_CD_LEFT           0x36  /* Cd audio level left */
#define MIXER_CD_RIGHT          0x37  /* Cd audio level right */
#define MIXER_LINE_LEFT         0x38  /* Line in level left */
#define MIXER_LINE_RIGHT        0x39  /* Line in level right */
#define MIXER_MIC_LEVEL         0x3A  /* Microphone level */
#define MIXER_PC_LEVEL          0x3B  /* Pc speaker level */
#define MIXER_OUTPUT_CTRL       0x3C  /* Output control */
#define MIXER_IN_LEFT           0x3D  /* Input control left */
#define MIXER_IN_RIGHT          0x3E  /* Input control right */
#define MIXER_GAIN_IN_LEFT      0x3F  /* Input gain control left */
#define MIXER_GAIN_IN_RIGHT     0x40  /* Input gain control right */
#define MIXER_GAIN_OUT_LEFT     0x41  /* Output gain control left */
#define MIXER_GAIN_OUT_RIGHT    0x42  /* Output gain control rigth */
#define MIXER_AGC               0x43  /* Automatic gain control */
#define MIXER_TREBLE_LEFT       0x44  /* Treble left */
#define MIXER_TREBLE_RIGHT      0x45  /* Treble right */
#define MIXER_BASS_LEFT         0x46  /* Bass left */
#define MIXER_BASS_RIGHT        0x47  /* Bass right */
#define MIXER_SET_IRQ           0x80  /* Set irq number */
#define MIXER_SET_DMA           0x81  /* Set DMA channels */
#define MIXER_IRQ_STATUS        0x82  /* Irq status */

/* Mixer constants */
#define MIC                     0x01  /* Microphone */
#define CD_RIGHT                0x02
#define CD_LEFT                 0x04
#define LINE_RIGHT              0x08
#define LINE_LEFT               0x10
#define FM_RIGHT                0x20
#define FM_LEFT                 0x40

/* DSP constants */
#define DMA_NR_OF_BUFFERS       2
#define DSP_MAX_SPEED           44100
#define DSP_DEFAULT_SPEED       22050
#define DSP_MIN_SPEED           4000
#define DSP_MAX_FRAGMENT_SIZE   ((64 * 1024) /  DMA_NR_OF_BUFFERS)
#define DSP_MIN_FRAGMENT_SIZE   1024
#define DSP_NR_OF_BUFFERS       8


class CSoundBlaster
 : public IInterruptServiceRoutine
{
public:
  CSoundBlaster();
  ~CSoundBlaster();

  int init();
  void start();

  // DSP
  bool    dspReset();
  bool    dspCommand(uint8_t value);
  bool    dspSetSampleRate(uint16_t speed);

  // Mixer
  uint8_t mixerGet(int reg);
  bool    mixerSet(int reg, uint8_t data);

  void write(const int16_t * sampleData, unsigned int sampleCount);

  // Inherited from IInterruptServiceRoutine
  virtual int isr(int irq);

private:
  uint8_t dspIn8(uint16_t addr);
  void    dspOut8(uint16_t addr, uint8_t data);

private:
  unsigned int   iFragmentSize_;

  void         * pDMABufferPhys_;
  void         * pDMABuffer_;
  unsigned int   iDMABufferSize_;
  unsigned int   iDMAFragmentCount_;
  volatile int   iDMARead_;
  volatile int   iDMAWrite_;
  unsigned int   iDMAFragmentOffset_;

#ifdef CONFIG_MULTI_THREADING
  pthread_mutex_t mutex_;       // The locking mutex
  pthread_cond_t condFragmentPlayed_;    // State change condition
#endif

  uint16_t iSampleRate_;
  bool     bStereo_;
  bool     bSigned_;
  int      iBits_;
};


#endif
