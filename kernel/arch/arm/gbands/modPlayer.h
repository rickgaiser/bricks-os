// Based on Deku's gba-mod tutorial: http://deku.gbadev.org/
#ifndef MODPLAYER_H
#define MODPLAYER_H


#include "gbaSound.h"
#include "inttypes.h"


#define MOD_CHANNEL_COUNT 4


enum MOD_STATE
{
  MOD_STATE_STOP,
  MOD_STATE_PLAY,
  MOD_STATE_PAUSE,
};

typedef void (*MOD_CALLBACK)(uint32_t param, bool bRowTick);

// -----------------------------------------------------------------------------
// This is the layout of the sample info in ROM
struct SAMPLE_HEADER
{
  uint16_t  length;
  uint8_t   finetune;
  uint8_t   vol;
  uint16_t  loopStart;
  uint16_t  loopLength;

  const int8_t * smpData;   // Pointer to sample data in ROM
};

// -----------------------------------------------------------------------------
// This is the MOD data layout in ROM
struct MOD_HEADER
{
  const SAMPLE_HEADER * sample;
  const uint8_t * order;
  const uint8_t ** pattern;

  uint8_t orderCount;
  uint8_t pad[3];
};

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
  CMODPlayer(CGBASound * driver);
  virtual ~CMODPlayer();

  int  init();
  void update();

private:
  void MODUpdate();
  void MODProcessRow();
  void MODUpdateEffects();
  void MODHandleUpdateFlags(SMODUpdateInfo *vars);
  void MODPlayNote(SMODUpdateInfo *vars);
  void MODPlay(MOD_HEADER const *modHeader);
  void MODStop();
  bool MODSeek(uint32_t order, uint32_t row);
  void MODSetTempo(uint32_t tempo);
  uint32_t  MODVolumeSlide(uint32_t volume, int32_t slide);
  int32_t  MODPitchSlide(int32_t period, int32_t slide);

  void MODInitVibrato(SMODUpdateInfo *vars, SMODVibrato *vibrato);
  void MODUpdateVibrato(SMODVibrato *vibrato);

  void MODFXArpeggioRow(SMODUpdateInfo *vars);
  void MODFXArpeggioMid(SMODUpdateInfo *vars);
  void MODFXPortaRow(SMODUpdateInfo *vars);
  void MODFXPortaDownMid(SMODUpdateInfo *vars);
  void MODFXPortaUpMid(SMODUpdateInfo *vars);
  void MODFXTonePortaRow(SMODUpdateInfo *vars);
  void MODFXTonePortaMid(SMODUpdateInfo *vars);
  void MODFXVibratoRow(SMODUpdateInfo *vars);
  void MODFXVibratoMid(SMODUpdateInfo *vars);
  void MODFXVSldTPortaRow(SMODUpdateInfo *vars);
  void MODFXVSldTPortaMid(SMODUpdateInfo *vars);
  void MODFXVSldVibratoRow(SMODUpdateInfo *vars);
  void MODFXVSldVibratoMid(SMODUpdateInfo *vars);
  void MODFXTremoloRow(SMODUpdateInfo *vars);
  void MODFXTremoloMid(SMODUpdateInfo *vars);
  void MODFXSampleOffset(SMODUpdateInfo *vars);
  void MODFXVolslideRow(SMODUpdateInfo *vars);
  void MODFXVolslideMid(SMODUpdateInfo *vars);
  void MODFXJumpToOrder(SMODUpdateInfo *vars);
  void MODFXSetVol(SMODUpdateInfo *vars);
  void MODFXBreakToRow(SMODUpdateInfo *vars);
  void MODFXSpecialRow(SMODUpdateInfo *vars);
  void MODFXSpecialMid(SMODUpdateInfo *vars);
  void MODFXSpeed(SMODUpdateInfo *vars);

private:
  CGBASound * driver_;
  SSoundChannel channel_[MOD_CHANNEL_COUNT];

  SMOD       mod_;

  uint32_t   mixBufferSize_;
  uint16_t   mixFreq_;
  uint32_t   mixFreqPeriod_;        // 12-bit fixed-point
  int32_t    samplesUntilMODTick_;  // 12-bit fixed-point
  uint32_t   samplesPerMODTick_;    // 12-bit fixed-point
  uint8_t    channelBlocked_;       // One bit per mixer channel
};


extern MOD_HEADER dModTable[];


#endif
