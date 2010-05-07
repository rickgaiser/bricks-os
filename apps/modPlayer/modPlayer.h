/*
 * Bricks-OS, Operating System for Game Consoles
 * Copyright (C) 2008 Maximus32 <Maximus32@bricks-os.org>
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
 *
 * Based on Deku's gba-mod tutorial: http://deku.gbadev.org/
 */


#ifndef MODPLAYER_H
#define MODPLAYER_H


#include "mod.h"
#include "inttypes.h"
#include "asm/arch/memory.h"
#include "../../../kernel/arch/arm/gbands/gbaSound.h"
#include "../../../kernel/arch/arm/gbands/drivers.h"


#define MOD_CHANNEL_COUNT 4
#define SOUND_CHANNEL_COUNT 4


enum MOD_STATE
{
  MOD_STATE_STOP,
  MOD_STATE_PLAY,
  MOD_STATE_PAUSE,
};

typedef void (*MOD_CALLBACK)(uint32_t param, bool bRowTick);

// -----------------------------------------------------------------------------
// Used in the SMODChannel struct below, to make recycling code for
// the vibrato and tremolo effects easier
struct SMODVibrato
{
  int8_t  slide;         // Ranges +-64. Vibrato needs to shift down 1 bit
  uint8_t speed  : 4;    // Added to tick each update
  uint8_t depth  : 4;    // Multiplied by table value, and shifted down
  uint8_t tick   : 6;    // Position in table. Full cycle is 64 ticks
  uint8_t waveform : 2;  // Type of vibration. See MOD_WAVEFORM in Sound.c
  uint8_t noRetrig : 1;  // If FALSE, reset tick to 0 when a new note is played
  uint8_t pad    : 7;    // Unused, align to 4 bytes
};

// -----------------------------------------------------------------------------
// This is the data we need to keep track of for each channel
// while playing the song, stored in RAM
struct SMODChannel
{
  uint16_t period;         // Current period of note being played

  uint8_t  note;            // Current note
  uint8_t  sample;          // Last sample used on this channel
  uint8_t  vol;             // Current volume
  uint8_t  finetune;        // Finetune level (0-15)

  uint8_t  effect;          // Current effect (set to 0 each row)
  uint8_t  param;           // Current param (set to 0 each row)

  int8_t   volslideSpeed;   // Current volume slide speed
  uint8_t  portaSpeed;      // Current pitch slide up/down speed
  uint8_t  tonePortaNote;   // Current note to slide toward
  uint8_t  tonePortaSpeed;  // Speed to slide toward it
  uint8_t  patLoopPos;      // Set to current row when an E60 effect is used
  uint8_t  patLoopCount;    // Number of times left to loop

  union
  {
    uint8_t  retrigTick;    // MOD ticks until note should retrigger
    uint8_t  noteCutTick;   // MOD ticks until note should cut
    uint8_t  noteDelayTick; // MOD ticks until note should play
    uint8_t  arpeggioTick;  // Cycles 0-2 for original note and arpeggio notes
  };

  SMODVibrato vibrato;      // Vibrates frequency
  SMODVibrato tremolo;      // Vibrates volume
};

// -----------------------------------------------------------------------------
// This is all the data for the currently playing song, in RAM
struct SMOD
{
  const SAMPLE_HEADER * sample; // Pointer to table of samples in ROM
  const uint8_t ** pattern;     // Pointer to table of pointers to patterns
  const uint8_t * order;        // Array of pattern numbers to play
  MOD_CALLBACK callback;        // User function called by 0xE0 effects

  const uint8_t * rowPtr;       // Current position in current pattern, for quick access

  uint8_t  state;             // MOD_STATE enum (stopped/playing/paused)

  uint8_t  speed;
  uint8_t  tick;              // When this gets to speed, process a new row and reset it to 0

  uint8_t  curRow;            // When this gets to 64, move to the next order and reset to 0

  uint8_t  orderCount;
  uint8_t  curOrder;          // When this gets to orderCount, stop the song
  uint8_t  nextOrder;         // Normally curOrder+1, unless an effect changes it
  uint8_t  breakRow;          // Starting row when order changes (normally 0)
  uint8_t  patDelay;          // Rows left to wait (normally 0)

  uint8_t  tempo;             // In BPM (Hz = BPM*2/5)

  uint8_t  pad[2];

  SMODChannel channel[MOD_CHANNEL_COUNT]; // Current state of each channel
};

// -----------------------------------------------------------------------------
struct SSoundChannel
{
  const int8_t * data;
  uint32_t pos;
  uint32_t inc;
  uint32_t vol;
  uint32_t length;
  uint32_t loopLength;
};

// -----------------------------------------------------------------------------
class CSoundMixer
{
public:
  CSoundMixer();
  virtual ~CSoundMixer();

  int init();

  void setChannel(uint8_t nr, SSoundChannel * data);
  void mix(uint32_t samplesToMix) FAST_CODE;

private:
  // Input channels
  SSoundChannel * channel_[SOUND_CHANNEL_COUNT];

  // Hardware output channels
  CGBASoundChannel channelA_;
  //CGBASoundChannel channelB_;
};

// -----------------------------------------------------------------------------
struct SMODUpdateInfo
{
  SMODChannel   * modChn;
  SSoundChannel * sndChn;

  uint8_t note;
  uint8_t sample;
  uint8_t effect;
  uint8_t param;

  uint8_t updateFlags;
  int8_t  fineSlide;
  uint8_t smpOffset;
};

// -----------------------------------------------------------------------------
class CMODPlayer
{
public:
  CMODPlayer();
  virtual ~CMODPlayer();

  int  init();
  void play(MOD_HEADER const * modHeader);
  void stop();
  void update();

private:
  void MODUpdate();
  void MODProcessRow();
  void MODUpdateEffects();
  void MODHandleUpdateFlags(SMODUpdateInfo *vars);
  void MODPlayNote(SMODUpdateInfo *vars);
  bool MODSeek(uint32_t order, uint32_t row);
  void MODSetTempo(uint32_t tempo);
  uint32_t  MODVolumeSlide(uint32_t volume, int32_t slide);
  int32_t  MODPitchSlide(int32_t period, int32_t slide);

  void MODInitVibrato(SMODUpdateInfo *vars, SMODVibrato *vibrato);
  void MODUpdateVibrato(SMODVibrato *vibrato);

  void fxArpeggioRow(SMODUpdateInfo *vars);
  void fxArpeggioMid(SMODUpdateInfo *vars);
  void fxPortaRow(SMODUpdateInfo *vars);
  void fxPortaDownMid(SMODUpdateInfo *vars);
  void fxPortaUpMid(SMODUpdateInfo *vars);
  void fxTonePortaRow(SMODUpdateInfo *vars);
  void fxTonePortaMid(SMODUpdateInfo *vars);
  void fxVibratoRow(SMODUpdateInfo *vars);
  void fxVibratoMid(SMODUpdateInfo *vars);
  void fxVSldTPortaRow(SMODUpdateInfo *vars);
  void fxVSldTPortaMid(SMODUpdateInfo *vars);
  void fxVSldVibratoRow(SMODUpdateInfo *vars);
  void fxVSldVibratoMid(SMODUpdateInfo *vars);
  void fxTremoloRow(SMODUpdateInfo *vars);
  void fxTremoloMid(SMODUpdateInfo *vars);
  void fxSampleOffset(SMODUpdateInfo *vars);
  void fxVolslideRow(SMODUpdateInfo *vars);
  void fxVolslideMid(SMODUpdateInfo *vars);
  void fxJumpToOrder(SMODUpdateInfo *vars);
  void fxSetVol(SMODUpdateInfo *vars);
  void fxBreakToRow(SMODUpdateInfo *vars);
  void fxSpecialRow(SMODUpdateInfo *vars);
  void fxSpecialMid(SMODUpdateInfo *vars);
  void fxSpeed(SMODUpdateInfo *vars);

private:
  CSoundMixer   mixer_;
  SSoundChannel channel_[MOD_CHANNEL_COUNT];

  SMOD       mod_;

  uint32_t   mixBufferSize_;
  uint16_t   mixFreq_;
  uint32_t   mixFreqPeriod_;        // 12-bit fixed-point
  int32_t    samplesUntilMODTick_;  // 12-bit fixed-point
  uint32_t   samplesPerMODTick_;    // 12-bit fixed-point
  uint8_t    channelBlocked_;       // One bit per mixer channel
};


#endif
