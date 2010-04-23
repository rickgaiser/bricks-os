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


#include "modPlayer.h"
#include "stdlib.h"
#include "string.h"


// For patterns to specify that there is no note. We have 5 octaves,
// so only notes 0-59 are used, and 63 is the highest that still fits
// in the same number of bits
#define MOD_NO_NOTE          63
// Valid samples are 0-30, use 31 as a blank that still fits in 5 bits
#define MOD_NO_SAMPLE        31

#define MOD_DEFAULT_SPEED     6 // Starting speed is 6 ticks per row
#define MOD_DEFAULT_TEMPO   125 // Starting tempo is 125 bpm
#define MOD_PATTERN_ROWS     64 // All patterns are 64 rows

#define MOD_MAX_NOTE         59 // Highest valid note
#define MOD_PERIOD_MAX     1814
#define MOD_PERIOD_MIN       53

#define MIXCHN_FRAC_BITS     12

#define AMIGA_VAL       3579545

#define div(a,b) ((a)/(b))


enum MOD_UPDATE_FLAGS
{
  MOD_UPD_FLG_SET_VOL   = (1<<0), // Row or non-row, recalculates mix chn volume
  MOD_UPD_FLG_SET_FREQ  = (1<<1), // Row or non-row, recalculates mix chn increment
  MOD_UPD_FLG_PLAY_NOTE = (1<<2), // Used on row-ticks to play or skip note
};

enum MOD_EFFECT_TABLE
{
  MOD_EFFECT_TABLE_ROW,
  MOD_EFFECT_TABLE_MID,
  MOD_EFFECT_TABLE_NUM,
};

enum MOD_WAVEFORM
{
  MOD_WAVEFORM_SINE,
  MOD_WAVEFORM_RAMP,
  MOD_WAVEFORM_SQUARE,
  MOD_WAVEFORM_RANDOM,
  MOD_WAVEFORM_NUM,
};

// Save ourselves the trouble of initializing each member in code
const SMODUpdateInfo modDefaultVars[MOD_EFFECT_TABLE_NUM] =
{
  { // MOD_EFFECT_TABLE_ROW
    NULL,          // modChn
    NULL,          // sndChn
    MOD_NO_NOTE,   // note
    MOD_NO_SAMPLE, // sample
    0,             // effect
    0,             // param
    MOD_UPD_FLG_PLAY_NOTE, // updateFlags
    0,             // fineSlide
    0,             // smpOffset
  },
  { // MOD_EFFECT_TABLE_MID
    NULL,          // modChn
    NULL,          // sndChn
    MOD_NO_NOTE,   // note
    MOD_NO_SAMPLE, // sample
    0,             // effect
    0,             // param
    0,             // updateFlags
    0,             // fineSlide
    0,             // smpOffset
  }
};

const uint16_t notePeriodTable[] =
{
  // Finetune 0
  1712,  1616,  1524,  1440,  1356,  1280,  1208,  1140,  1076,  1016,   960,   906, // C0-B0
   856,   808,   762,   720,   678,   640,   604,   570,   538,   508,   480,   453, // C1-B1
   428,   404,   381,   360,   339,   320,   302,   285,   269,   254,   240,   226, // C2-B2
   214,   202,   190,   180,   169,   160,   151,   142,   134,   127,   120,   113, // C3-B3
   107,   101,    95,    90,    84,    80,    75,    71,    67,    63,    60,    56, // C4-B4
  // Finetune 1
  1700,  1604,  1514,  1430,  1348,  1274,  1202,  1134,  1070,  1010,   954,   900, // C0-B0
   850,   802,   757,   715,   674,   637,   601,   567,   535,   505,   477,   450, // C1-B1
   425,   401,   378,   357,   337,   318,   300,   283,   267,   252,   238,   225, // C2-B2
   212,   200,   189,   178,   168,   159,   150,   141,   133,   126,   119,   112, // C3-B3
   106,   100,    94,    89,    84,    79,    75,    70,    66,    63,    59,    56, // C4-B4
  // Finetune 2
  1688,  1592,  1504,  1418,  1340,  1264,  1194,  1126,  1064,  1004,   948,   894, // C0-B0
   844,   796,   752,   709,   670,   632,   597,   563,   532,   502,   474,   447, // C1-B1
   422,   398,   376,   354,   335,   316,   298,   281,   266,   251,   237,   223, // C2-B2
   211,   199,   188,   177,   167,   158,   149,   140,   133,   125,   118,   111, // C3-B3
   105,    99,    94,    88,    83,    79,    74,    70,    66,    62,    59,    55, // C4-B4
  // Finetune 3
  1676,  1582,  1492,  1408,  1330,  1256,  1184,  1118,  1056,   996,   940,   888, // C0-B0
   838,   791,   746,   704,   665,   628,   592,   559,   528,   498,   470,   444, // C1-B1
   419,   395,   373,   352,   332,   314,   296,   279,   264,   249,   235,   222, // C2-B2
   209,   197,   186,   176,   166,   157,   148,   139,   132,   124,   117,   111, // C3-B3
   104,    98,    93,    88,    83,    78,    74,    69,    66,    62,    58,    55, // C4-B4
  // Finetune 4
  1664,  1570,  1482,  1398,  1320,  1246,  1176,  1110,  1048,   990,   934,   882, // C0-B0
   832,   785,   741,   699,   660,   623,   588,   555,   524,   495,   467,   441, // C1-B1
   416,   392,   370,   349,   330,   311,   294,   277,   262,   247,   233,   220, // C2-B2
   208,   196,   185,   174,   165,   155,   147,   138,   131,   123,   116,   110, // C3-B3
   104,    98,    92,    87,    82,    77,    73,    69,    65,    61,    58,    55, // C4-B4
  // Finetune 5
  1652,  1558,  1472,  1388,  1310,  1238,  1168,  1102,  1040,   982,   926,   874, // C0-B0
   826,   779,   736,   694,   655,   619,   584,   551,   520,   491,   463,   437, // C1-B1
   413,   389,   368,   347,   327,   309,   292,   275,   260,   245,   231,   218, // C2-B2
   206,   194,   184,   173,   163,   154,   146,   137,   130,   122,   115,   109, // C3-B3
   103,    97,    92,    86,    81,    77,    73,    68,    65,    61,    57,    54, // C4-B4
  // Finetune 6
  1640,  1548,  1460,  1378,  1302,  1228,  1160,  1094,  1032,   974,   920,   868, // C0-B0
   820,   774,   730,   689,   651,   614,   580,   547,   516,   487,   460,   434, // C1-B1
   410,   387,   365,   344,   325,   307,   290,   273,   258,   243,   230,   217, // C2-B2
   205,   193,   182,   172,   162,   153,   145,   136,   129,   121,   115,   108, // C3-B3
   102,    96,    91,    86,    81,    76,    72,    68,    64,    60,    57,    54, // C4-B4
  // Finetune 7
  1628,  1536,  1450,  1368,  1292,  1220,  1150,  1086,  1026,   968,   914,   862, // C0-B0
   814,   768,   725,   684,   646,   610,   575,   543,   513,   484,   457,   431, // C1-B1
   407,   384,   362,   342,   323,   305,   287,   271,   256,   242,   228,   215, // C2-B2
   203,   192,   181,   171,   161,   152,   143,   135,   128,   121,   114,   107, // C3-B3
   101,    96,    90,    85,    80,    76,    71,    67,    64,    60,    57,    53, // C4-B4
  // Finetune 8
  1814,  1712,  1616,  1524,  1440,  1356,  1280,  1208,  1140,  1076,  1016,   960, // C0-B0
   907,   856,   808,   762,   720,   678,   640,   604,   570,   538,   508,   480, // C1-B1
   453,   428,   404,   381,   360,   339,   320,   302,   285,   269,   254,   240, // C2-B2
   226,   214,   202,   190,   180,   169,   160,   151,   142,   134,   127,   120, // C3-B3
   113,   107,   101,    95,    90,    84,    80,    75,    71,    67,    63,    60, // C4-B4
  // Finetune 9
  1800,  1700,  1604,  1514,  1430,  1350,  1272,  1202,  1134,  1070,  1010,   954, // C0-B0
   900,   850,   802,   757,   715,   675,   636,   601,   567,   535,   505,   477, // C1-B1
   450,   425,   401,   378,   357,   337,   318,   300,   283,   267,   252,   238, // C2-B2
   225,   212,   200,   189,   178,   168,   159,   150,   141,   133,   126,   119, // C3-B3
   112,   106,   100,    94,    89,    84,    79,    75,    70,    66,    63,    59, // C4-B4
  // Finetune 10
  1788,  1688,  1592,  1504,  1418,  1340,  1264,  1194,  1126,  1064,  1004,   948, // C0-B0
   894,   844,   796,   752,   709,   670,   632,   597,   563,   532,   502,   474, // C1-B1
   447,   422,   398,   376,   354,   335,   316,   298,   281,   266,   251,   237, // C2-B2
   223,   211,   199,   188,   177,   167,   158,   149,   140,   133,   125,   118, // C3-B3
   111,   105,    99,    94,    88,    83,    79,    74,    70,    66,    62,    59, // C4-B4
  // Finetune 11
  1774,  1676,  1582,  1492,  1408,  1330,  1256,  1184,  1118,  1056,   996,   940, // C0-B0
   887,   838,   791,   746,   704,   665,   628,   592,   559,   528,   498,   470, // C1-B1
   443,   419,   395,   373,   352,   332,   314,   296,   279,   264,   249,   235, // C2-B2
   221,   209,   197,   186,   176,   166,   157,   148,   139,   132,   124,   117, // C3-B3
   110,   104,    98,    93,    88,    83,    78,    74,    69,    66,    62,    58, // C4-B4
  // Finetune 12
  1762,  1664,  1570,  1482,  1398,  1320,  1246,  1176,  1110,  1048,   988,   934, // C0-B0
   881,   832,   785,   741,   699,   660,   623,   588,   555,   524,   494,   467, // C1-B1
   440,   416,   392,   370,   349,   330,   311,   294,   277,   262,   247,   233, // C2-B2
   220,   208,   196,   185,   174,   165,   155,   147,   138,   131,   123,   116, // C3-B3
   110,   104,    98,    92,    87,    82,    77,    73,    69,    65,    61,    58, // C4-B4
  // Finetune 13
  1750,  1652,  1558,  1472,  1388,  1310,  1238,  1168,  1102,  1040,   982,   926, // C0-B0
   875,   826,   779,   736,   694,   655,   619,   584,   551,   520,   491,   463, // C1-B1
   437,   413,   389,   368,   347,   327,   309,   292,   275,   260,   245,   231, // C2-B2
   218,   206,   194,   184,   173,   163,   154,   146,   137,   130,   122,   115, // C3-B3
   109,   103,    97,    92,    86,    81,    77,    73,    68,    65,    61,    57, // C4-B4
  // Finetune 14
  1736,  1640,  1548,  1460,  1378,  1302,  1228,  1160,  1094,  1032,   974,   920, // C0-B0
   868,   820,   774,   730,   689,   651,   614,   580,   547,   516,   487,   460, // C1-B1
   434,   410,   387,   365,   344,   325,   307,   290,   273,   258,   243,   230, // C2-B2
   217,   205,   193,   182,   172,   162,   153,   145,   136,   129,   121,   115, // C3-B3
   108,   102,    96,    91,    86,    81,    76,    72,    68,    64,    60,    57, // C4-B4
  // Finetune 15
  1724,  1628,  1536,  1450,  1368,  1292,  1220,  1150,  1086,  1026,   968,   914, // C0-B0
   862,   814,   768,   725,   684,   646,   610,   575,   543,   513,   484,   457, // C1-B1
   431,   407,   384,   362,   342,   323,   305,   287,   271,   256,   242,   228, // C2-B2
   215,   203,   192,   181,   171,   161,   152,   143,   135,   128,   121,   114, // C3-B3
   107,   101,    96,    90,    85,    80,    76,    71,    67,    64,    60,    57, // C4-B4
};

const int8_t vibratoSineTab[32] =
{
    0,  24,  49,  74,  97, 120, 141, 161,
  180, 197, 212, 224, 235, 244, 250, 253,
  255, 253, 250, 244, 235, 224, 212, 197,
  180, 161, 141, 120,  97,  74,  49,  24
};

const int8_t vibratoRandomTab[32] =
{
  -111,  43,  -13, -65,  50,  83, 114,  -58,
    34, -42, -101, 127,  74, -22,  25,  120,
   -30,  -7,   12, 104, -46,   7,  19,  -97,
    57, -79, -115,  61, -72,   0,  90, -128
};


// -----------------------------------------------------------------------------
CMODPlayer::CMODPlayer()
 : mixer_()
{
}

// -----------------------------------------------------------------------------
CMODPlayer::~CMODPlayer()
{
}

// -----------------------------------------------------------------------------
int
CMODPlayer::init()
{
  mixBufferSize_ = 528;//freqTable[freq].bufSize;
  mixFreq_       = 31536;//freqTable[freq].freq;
  mixFreqPeriod_ = div(AMIGA_VAL<<9, mixFreq_) << (MIXCHN_FRAC_BITS-9);

  mixer_.init();

  for(int iChannel(0); iChannel < MOD_CHANNEL_COUNT; iChannel++)
  {
    channel_[iChannel].data       = NULL;
    channel_[iChannel].pos        = 0;
    channel_[iChannel].inc        = 0;
    channel_[iChannel].vol        = 0;
    channel_[iChannel].length     = 0;
    channel_[iChannel].loopLength = 0;
  }

  MODPlay(&dModTable[0]);

  return 0;
}

// -----------------------------------------------------------------------------
void
CMODPlayer::update()
{
  int32_t samplesLeft = mixBufferSize_;

  while(samplesLeft > 0)
  {
    // Check if the song needs updated
    if(samplesUntilMODTick_ < (1<<12) && mod_.state == MOD_STATE_PLAY)
    {
      // Update the song and set the number of samples to mix before the next update
      MODUpdate();
      samplesUntilMODTick_ += samplesPerMODTick_;
    }

    // Figure out if this is the last batch of samples for this frame
    if((samplesUntilMODTick_>>12) < samplesLeft && mod_.state == MOD_STATE_PLAY)
    {
      mixer_.setChannel(0, &channel_[0]);
      mixer_.setChannel(1, &channel_[1]);
      mixer_.setChannel(2, &channel_[2]);
      mixer_.setChannel(3, &channel_[3]);
      // Song will need updated before we're out of samples, so mix up to the song tick
      mixer_.mix(samplesUntilMODTick_>>12);
      // Subtract the number we just mixed
      samplesLeft -= samplesUntilMODTick_>>12;

      // No more left, so song will get updated next time through the loop
      samplesUntilMODTick_ -= (samplesUntilMODTick_>>12)<<12;
    }
    else
    {
      mixer_.setChannel(0, &channel_[0]);
      mixer_.setChannel(1, &channel_[1]);
      mixer_.setChannel(2, &channel_[2]);
      mixer_.setChannel(3, &channel_[3]);
      // Either song is not playing, so just mix a full buffer, or
      // not enough samples left to make it to another song tick,
      // so mix what's left and exit
      mixer_.mix(samplesLeft);
      // This is how many samples will get mixed first thing next frame
      // before updating the song
      samplesUntilMODTick_ -= samplesLeft<<12;
      // Mixed the last of the 304 samples, this will break from the while loop
      samplesLeft = 0;
    }
  }

  // Free up SFX channels that ended
  for(int iChannel(0); iChannel < MOD_CHANNEL_COUNT; iChannel++)
  {
    if((channelBlocked_ & (1<<iChannel)) && channel_[iChannel].data == NULL)
    {
      channelBlocked_ &= ~(1<<iChannel);
    }
  }
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODUpdate()
{
  if(mod_.state != MOD_STATE_PLAY)
    return;

  if(++mod_.tick >= mod_.speed)
  {
    mod_.tick = 0;

    if(mod_.patDelay == 0)
    {
      if(mod_.curRow++ >= MOD_PATTERN_ROWS)
      {
        if(MODSeek(mod_.nextOrder, mod_.breakRow) == false)
          return;

        mod_.curRow++;
      }

      MODProcessRow();
    }
    else
    {
      mod_.patDelay--;
    }
  }
  else
  {
    MODUpdateEffects();
  }
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODProcessRow()
{
  for(int iChannel(0); iChannel < MOD_CHANNEL_COUNT; iChannel++)
  {
    SMODUpdateInfo vars = modDefaultVars[MOD_EFFECT_TABLE_ROW];

    vars.modChn = &mod_.channel[iChannel];
    vars.sndChn = &channel_[iChannel];

    // Read in the pattern data, advancing rowPtr to the next channel in the process
    vars.note   = *mod_.rowPtr++;
    vars.sample = *mod_.rowPtr++;
    vars.effect = *mod_.rowPtr++;
    vars.param  = *mod_.rowPtr++;

    // Set these for the mid-ticks
    vars.modChn->note   = vars.note;
    vars.modChn->effect = vars.effect;
    vars.modChn->param  = vars.param;

    // Set sample and volume BEFORE effect processing, because some
    // effects read the sample from the MOD channel rather than vars,
    // and others need to override the default volume
    if(vars.sample != MOD_NO_SAMPLE)   // Never set local to memory anymore (explained below)
    {
      // Set sample memory
      vars.modChn->sample     = vars.sample;
      vars.modChn->vol        = mod_.sample[vars.sample].vol;
      vars.modChn->finetune   = mod_.sample[vars.sample].finetune;
      vars.updateFlags       |= MOD_UPD_FLG_SET_VOL;
    }

      // Effect 0 is arpeggio, but is also used as no-effect if the param is 0 too
    if((vars.effect|vars.param) != 0)
    {
      switch(vars.effect)
      {
        case 0x0: MODFXArpeggioRow(&vars); break;
        case 0x1: MODFXPortaRow(&vars); break;
        case 0x2: MODFXPortaRow(&vars); break;
        case 0x3: MODFXTonePortaRow(&vars); break;
        case 0x4: MODFXVibratoRow(&vars); break;
        case 0x5: MODFXVSldTPortaRow(&vars); break;
        case 0x6: MODFXVSldVibratoRow(&vars); break;
        case 0x7: MODFXTremoloRow(&vars); break;
        // 0x8: Set panning (unsupported)
        case 0x9: MODFXSampleOffset(&vars); break;
        case 0xa: MODFXVolslideRow(&vars); break;
        case 0xb: MODFXJumpToOrder(&vars); break;
        case 0xc: MODFXSetVol(&vars); break;
        case 0xd: MODFXBreakToRow(&vars); break;
        case 0xe: MODFXSpecialRow(&vars); break;
        case 0xf: MODFXSpeed(&vars); break;
        default:
          ;
      };
    }

    if( !(channelBlocked_ & (1<<iChannel)) )
      MODHandleUpdateFlags(&vars);
  }
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODUpdateEffects()
{
  for(int iChannel(0); iChannel < MOD_CHANNEL_COUNT; iChannel++)
  {
    if(mod_.channel[iChannel].effect|mod_.channel[iChannel].param != 0)
    {
      SMODUpdateInfo vars = modDefaultVars[MOD_EFFECT_TABLE_MID];

      vars.modChn = &mod_.channel[iChannel];
      vars.sndChn = &channel_[iChannel];

      switch(vars.modChn->effect)
      {
        case 0x0: MODFXArpeggioMid(&vars); break;
        case 0x1: MODFXPortaUpMid(&vars); break;
        case 0x2: MODFXPortaDownMid(&vars); break;
        case 0x3: MODFXTonePortaMid(&vars); break;
        case 0x4: MODFXVibratoMid(&vars); break;
        case 0x5: MODFXVSldTPortaMid(&vars); break;
        case 0x6: MODFXVSldVibratoMid(&vars); break;
        case 0x7: MODFXTremoloMid(&vars); break;
        // 0x8: Set panning
        // 0x9: Sample offset
        case 0xa: MODFXVolslideMid(&vars); break;
        // 0xB: Jump to order
        // 0xC: Set volume
        // 0xD: Break to row
        case 0xe: MODFXSpecialMid(&vars); break;
        // 0xF: Speed/Tempo
        default:
          ;
      };

      if(!(channelBlocked_ & (1<<iChannel)))
        MODHandleUpdateFlags(&vars);
    }
  }
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODHandleUpdateFlags(SMODUpdateInfo *vars)
{
  if(vars->updateFlags & MOD_UPD_FLG_SET_VOL)
  {
    // Temporary volume to apply tremolo if needed
    uint32_t vol = vars->modChn->vol;
    if(vars->modChn->tremolo.slide != 0)
      vol = MODVolumeSlide(vol, vars->modChn->tremolo.slide);
    vars->sndChn->vol = vol;
  }

  if((vars->note != MOD_NO_NOTE) && (vars->updateFlags & MOD_UPD_FLG_PLAY_NOTE))
  {
    MODPlayNote(vars);
  }

  if(vars->fineSlide != 0)
  {
    // This has to happen after the note is played
    vars->modChn->period = MODPitchSlide(vars->modChn->period, vars->fineSlide);
    vars->updateFlags |= MOD_UPD_FLG_SET_FREQ;
  }

  if(vars->updateFlags & MOD_UPD_FLG_SET_FREQ)
  {
    uint32_t period = vars->modChn->period;
    // Shift down 1 bit here because the slide range needs to be
    // +-32 MOD periods, but the slide variable is +-64 because
    // tremolo reuses the same code and needs the larger range
    if(vars->modChn->vibrato.slide != 0)
      period = MODPitchSlide(period, vars->modChn->vibrato.slide >> 1);

    // mixFreqPeroid is already shifted up 12 bits for fixed-point
    vars->sndChn->inc = div(mixFreqPeriod_, period);
  }
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODPlayNote(SMODUpdateInfo *vars)
{
  const SAMPLE_HEADER *sample;

  // Here's that special case that they didn't specify a sample before playing a note
  if(vars->modChn->sample == MOD_NO_SAMPLE)
  {
    return;
  }

  // Handy
  sample = &mod_.sample[vars->modChn->sample];

  // 60 notes total, and one full set of notes for each finetune level
  vars->modChn->period     = notePeriodTable[vars->modChn->finetune*60 + vars->modChn->note];

  // Set up the mixer channel
  vars->sndChn->data       = sample->smpData;
  vars->sndChn->pos        = vars->smpOffset << (MIXCHN_FRAC_BITS+8);
  vars->updateFlags |= MOD_UPD_FLG_SET_FREQ;

  // Next member is volume, but skip setting it because it doesn't change unless a
  // new sample was specified, in which case it was already set back in MODProcessRow

  // If looping, use loopStart+loopLength as the ending point,
  // otherwise just use normal length.
  // Length and loop length are still half what the real size is
  // (to fit in 16 bits, as per MOD format), so they need to be
  // shifted left 1. They also need to be shifted left 12 because
  // the mixer compares the 12-bit fixed-point position against
  // them directly, so that comes to a total shift left of 13
  vars->sndChn->length = (sample->loopLength != 0 ?
                sample->loopStart + sample->loopLength :
                sample->length) << (MIXCHN_FRAC_BITS+1);
  vars->sndChn->loopLength = sample->loopLength << (MIXCHN_FRAC_BITS+1);
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODPlay(MOD_HEADER const *modHeader)
{
  memset(&mod_, 0, sizeof(SMOD));

  mod_.sample     = modHeader->sample;
  mod_.pattern    = modHeader->pattern;
  mod_.order      = modHeader->order;
  mod_.orderCount = modHeader->orderCount;
  mod_.tick       = 0;
  mod_.speed      = MOD_DEFAULT_SPEED;
  mod_.state      = MOD_STATE_PLAY;

  // Set the order/row/rowPtr
  MODSeek(0, 0);
  // Set to default
  MODSetTempo(MOD_DEFAULT_TEMPO);
  // Update the MOD first thing next time the mixer is called
  samplesUntilMODTick_ = 0;
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODStop()
{
  for(int iChannel(0); iChannel < MOD_CHANNEL_COUNT; iChannel++)
    channel_[iChannel].data = NULL;

  mod_.state = MOD_STATE_STOP;
}

// -----------------------------------------------------------------------------
// Returns false if song ended, true if still playing
bool
CMODPlayer::MODSeek(uint32_t order, uint32_t row)
{
  mod_.curOrder = order;
  if(mod_.curOrder >= mod_.orderCount)
  {
    MODStop();      // Hit the end of the song, so stop it
    return false;   // false = song ended
  }

  mod_.curRow = row;
  if(mod_.curRow >= MOD_PATTERN_ROWS)
    mod_.curRow = 0;  // Row was too high, just set it to 0

  mod_.nextOrder = mod_.curOrder + 1;
  mod_.breakRow  = 0;
  mod_.rowPtr    = mod_.pattern[mod_.order[mod_.curOrder]] + mod_.curRow*4*MOD_CHANNEL_COUNT;  // 4 bytes/channel/row

  return true;      // true = continue playing
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODSetTempo(uint32_t tempo)
{
  mod_.tempo = tempo;

  samplesUntilMODTick_ -= samplesPerMODTick_;
  samplesPerMODTick_    = div(mixFreq_*5<<12, tempo*2);
  samplesUntilMODTick_ += samplesPerMODTick_;
}

// -----------------------------------------------------------------------------
// Returns volume after sliding up/down and clamping to (0,64)
uint32_t
CMODPlayer::MODVolumeSlide(uint32_t volume, int32_t slide)
{
  volume += slide;
  if(volume > 64)     // Since volume is unsigned, this covers
  {                   // sliding too far in either direction
    if(slide > 0)
      volume = 64;  // Must have gone above 64
    else
      volume = 0;   // Must have gone below 0 and wrapped
  }

  return volume;
}

// -----------------------------------------------------------------------------
// Returns period after sliding up/down and
// clamping to (MOD_PERIOD_MIN,MOD_PERRIOD_MAX)
int32_t
CMODPlayer::MODPitchSlide(int32_t period, int32_t slide)
{
  period += slide;
  if (period > MOD_PERIOD_MAX)
    period = MOD_PERIOD_MAX;
  else if (period < MOD_PERIOD_MIN)
    period = MOD_PERIOD_MIN;

  return period;
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODInitVibrato(SMODUpdateInfo *vars, SMODVibrato *vibrato)
{
  if(vars->param & 0xf != 0)
    vibrato->depth = vars->param & 0xf;
  if(vars->param >> 4 != 0)
    vibrato->speed = vars->param >> 4;
  if(vibrato->noRetrig == false && vars->note != MOD_NO_NOTE)
    vibrato->tick = 0;
}

// -----------------------------------------------------------------------------
// Updates vibrato/tremolo, setting vibrato->slide to the range (-64, 63)
void
CMODPlayer::MODUpdateVibrato(SMODVibrato *vibrato)
{
  // Increment the tick. All wave types use a cycle of 0-63 on
  // the tick. Since it's a 6-bit bitfield, it wraps automatically.
  vibrato->tick += vibrato->speed;
  //vibrato->tick &= 63;

  switch(vibrato->waveform)
  {
  case MOD_WAVEFORM_SINE:
    // Sine table is 0-255, and depth is 0-15. Shift 6 places off,
    // giving a range of 0-64 (actually slightly less), and add or
    // subtract it to get the final range +-64
    vibrato->slide = vibratoSineTab[vibrato->tick]*vibrato->depth >> 6;
    if(vibrato->tick >= 32)         // First 32 ticks are the
      vibrato->slide = -vibrato->slide; // positive half of the sine
    break;                  // wave, next 32 are negative
  case MOD_WAVEFORM_RAMP:
    // This is a linear ramp down, 64 ticks long. Subtract tick from
    // 32 to give a value of 32 at tick 0, and -31 at tick 63 (just
    // before wrapping), effectively creating a downward slide.
    // Divide by 8 for final range of +-64
    vibrato->slide = (32 - vibrato->tick)*vibrato->depth >> 3;
    break;
  case MOD_WAVEFORM_SQUARE:
    // Square wave. Alternate between higest and lowest values.
    // Since the final range needs to be +-64 and the depth ranges
    // 0-15, just add or subtract 4x the depth
    vibrato->slide = vibrato->depth << 2;
    if(vibrato->tick >= 32)
      vibrato->slide = -vibrato->slide;
    break;
  case MOD_WAVEFORM_RANDOM:
    // Since this table only ranges -128 to +127, shift down
    // one less than sine to give the same final range
    vibrato->slide = vibratoRandomTab[vibrato->tick]*vibrato->depth >> 5;
    break;
  }
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXArpeggioRow(SMODUpdateInfo *vars)
{
  vars->modChn->arpeggioTick = 0;
  // Cancel the effect if no note has been played on this channel
  if(vars->note == MOD_NO_NOTE)
    vars->modChn->effect = vars->modChn->param = 0;
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXArpeggioMid(SMODUpdateInfo *vars)
{
  uint32_t arpNote = 0;

  if(++vars->modChn->arpeggioTick > 2)
    vars->modChn->arpeggioTick = 0;

  switch(vars->modChn->arpeggioTick)
  {
    case 0: arpNote = vars->modChn->note;               break;
    case 1: arpNote = vars->modChn->note + (vars->modChn->param >> 4);  break;
    case 2: arpNote = vars->modChn->note + (vars->modChn->param & 0xf); break;
  }

  if(arpNote > MOD_MAX_NOTE)
    arpNote = MOD_MAX_NOTE;

  vars->modChn->period = notePeriodTable[vars->modChn->finetune*60 + arpNote];

  vars->updateFlags |= MOD_UPD_FLG_SET_FREQ;
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXPortaRow(SMODUpdateInfo *vars)
{
  if(vars->param != 0)
    vars->modChn->portaSpeed = vars->param;
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXPortaDownMid(SMODUpdateInfo *vars)
{
  vars->modChn->period =
    MODPitchSlide(vars->modChn->period, vars->modChn->portaSpeed);

  vars->updateFlags |= MOD_UPD_FLG_SET_FREQ;
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXPortaUpMid(SMODUpdateInfo *vars)
{
  vars->modChn->period =
    MODPitchSlide(vars->modChn->period, -vars->modChn->portaSpeed);

  vars->updateFlags |= MOD_UPD_FLG_SET_FREQ;
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXTonePortaRow(SMODUpdateInfo *vars)
{
  if(vars->modChn->sample != MOD_NO_SAMPLE)
  {
    if(vars->note != MOD_NO_NOTE)
      vars->modChn->tonePortaNote = vars->note;
    // Else use the note like it is

    if(vars->param != 0)
      vars->modChn->tonePortaSpeed = vars->param;
    // Else use the speed like it is

    // Don't play the note or we wouldn't have anything to slide to
    vars->updateFlags &= ~MOD_UPD_FLG_PLAY_NOTE;
  }
  else  // No sample. Can't read finetune on mid-ticks, so cancel the effect
  {
    vars->modChn->effect = vars->modChn->param = 0;
  }
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXTonePortaMid(SMODUpdateInfo *vars)
{
  uint32_t tonePortaDestFreq;

  // Fetch the frequency of the target note
  tonePortaDestFreq = notePeriodTable[vars->modChn->finetune*60 + vars->modChn->tonePortaNote];

  if(vars->modChn->period < tonePortaDestFreq)
  {
    // Slide down toward note
    vars->modChn->period = MODPitchSlide(
      vars->modChn->period, vars->modChn->tonePortaSpeed);
    if(vars->modChn->period > tonePortaDestFreq)
    {
      // Slid too far, back up to the destination
      vars->modChn->period = tonePortaDestFreq;
    }
  }
  else if(vars->modChn->period > tonePortaDestFreq)
  {
    // Slide up toward note
    vars->modChn->period = MODPitchSlide(
      vars->modChn->period, -vars->modChn->tonePortaSpeed);
    if(vars->modChn->period < tonePortaDestFreq)
    {
      // Slid too far, back down to the destination
      vars->modChn->period = tonePortaDestFreq;
    }
  }
  // Else we're already at the target frequency

  // Mix channel increment will need recalculated
  vars->updateFlags |= MOD_UPD_FLG_SET_FREQ;
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXVibratoRow(SMODUpdateInfo *vars)
{
  MODInitVibrato(vars, &vars->modChn->vibrato);
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXVibratoMid(SMODUpdateInfo *vars)
{
  MODUpdateVibrato(&vars->modChn->vibrato);
  vars->updateFlags |= MOD_UPD_FLG_SET_FREQ;
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXVSldTPortaRow(SMODUpdateInfo *vars)
{
  // Param goes with the volslide part, tone porta just continues
  // So handle volslide like normal
  MODFXVolslideRow(vars);
  // Now trick the tone porta into thinking there was a
  // 0 'continue' param. This local param won't be used again,
  // so changing it won't hurt anything
  vars->param = 0;
  MODFXTonePortaRow(vars);
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXVSldTPortaMid(SMODUpdateInfo *vars)
{
  MODFXVolslideMid(vars);
  MODFXTonePortaMid(vars);
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXVSldVibratoRow(SMODUpdateInfo *vars)
{
  // Param goes with the volslide part, tone porta just continues
  // So handle volslide like normal
  MODFXVolslideRow(vars);
  // Now trick the vibrato into thinking there was a
  // 0 'continue' param. This local param won't be used again,
  // so changing it won't hurt anything
  vars->param = 0;
  MODFXVibratoRow(vars);
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXVSldVibratoMid(SMODUpdateInfo *vars)
{
  MODFXVolslideMid(vars);
  MODFXVibratoMid(vars);
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXTremoloRow(SMODUpdateInfo *vars)
{
  MODInitVibrato(vars, &vars->modChn->tremolo);
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXTremoloMid(SMODUpdateInfo *vars)
{
  MODUpdateVibrato(&vars->modChn->tremolo);
  vars->updateFlags |= MOD_UPD_FLG_SET_VOL;
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXSampleOffset(SMODUpdateInfo *vars)
{
  vars->smpOffset = vars->param;
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXVolslideRow(SMODUpdateInfo *vars)
{
  if(vars->param != 0)
  {
    if((vars->param & 0x0f) == 0)
      vars->modChn->volslideSpeed = vars->param >> 4;
    else if((vars->param & 0xf0) == 0)
      vars->modChn->volslideSpeed = -vars->param;
    else
      vars->modChn->effect = vars->modChn->param = 0;
  }
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXVolslideMid(SMODUpdateInfo *vars)
{
  vars->modChn->vol =
    MODVolumeSlide(vars->modChn->vol, vars->modChn->volslideSpeed);
  vars->updateFlags |= MOD_UPD_FLG_SET_VOL;
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXJumpToOrder(SMODUpdateInfo *vars)
{
  mod_.nextOrder = vars->param;
  mod_.curRow = MOD_PATTERN_ROWS; // Break next update
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXSetVol(SMODUpdateInfo *vars)
{
  vars->modChn->vol = vars->param;
  if(vars->modChn->vol > 64)
    vars->modChn->vol = 64;
  vars->updateFlags |= MOD_UPD_FLG_SET_VOL;
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXBreakToRow(SMODUpdateInfo *vars)
{
  mod_.breakRow = vars->param;
  mod_.curRow = MOD_PATTERN_ROWS; // Break next update
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXSpecialRow(SMODUpdateInfo *vars)
{
  // Since the upper 4 bits of the parameter are used as the effect
  // type, this saves us having to extract the lower bits every time
  uint32_t param = vars->modChn->param & 0xf;

  switch(vars->modChn->param >> 4)
  {
  case 0x0:   // Undefined, we use it for callback
    if(mod_.callback != NULL)
      mod_.callback(param, true);
    break;
  case 0x1:   // Fine porta up
    vars->fineSlide = param;
    vars->updateFlags |= MOD_UPD_FLG_SET_FREQ;
    break;
  case 0x2:   // Fine porta down
    vars->fineSlide = -param;
    vars->updateFlags |= MOD_UPD_FLG_SET_FREQ;
    break;
  case 0x3:   // Glissando (unsupported)
    break;
  case 0x4:   // Vibrato waveform
    vars->modChn->vibrato.waveform =  param & 3;
    vars->modChn->vibrato.noRetrig = (param & 4) ? true : false;
    break;
  case 0x5:   // Set finetune
    vars->modChn->finetune = param;
    if (vars->modChn->note != MOD_NO_NOTE)
    {
      vars->modChn->period = notePeriodTable[vars->modChn->finetune*60 +
                           vars->modChn->note];
      vars->updateFlags |= MOD_UPD_FLG_SET_FREQ;
    }
    break;
  case 0x6:   // Pattern loop
    if(param == 0)
      vars->modChn->patLoopPos = mod_.curRow;
    else
    {
      if(vars->modChn->patLoopCount == 0)
        vars->modChn->patLoopCount = param+1;

      if(--vars->modChn->patLoopCount != 0)
      { // Loop back to the stored row in this order next tick
        mod_.breakRow  = vars->modChn->patLoopPos;
        mod_.nextOrder = mod_.curOrder;   // Don't advance the order
        mod_.curRow    = MOD_PATTERN_ROWS;  // This triggers the break
      }
    }
    break;
  case 0x7:   // Tremolo waveform
    vars->modChn->tremolo.waveform =  param & 3;
    vars->modChn->tremolo.noRetrig = (param & 4) ? true : false;
    break;
  case 0x8:   // Set panning (unsupported)
    break;
  case 0x9:   // Retrigger note
    vars->modChn->retrigTick = param;
    break;
  case 0xA:   // Fine volume slide up
    vars->modChn->vol = MODVolumeSlide(vars->modChn->vol, param);
    vars->updateFlags |= MOD_UPD_FLG_SET_VOL;
    break;
  case 0xB:   // Fine volume slide down
    vars->modChn->vol = MODVolumeSlide(vars->modChn->vol, -param);
    vars->updateFlags |= MOD_UPD_FLG_SET_VOL;
    break;
  case 0xC:   // Note cut
    vars->modChn->noteCutTick = param;
    break;
  case 0xD:   // Note delay
    vars->modChn->noteDelayTick = param;
      // Don't set the mixer channel vol until the note plays
    vars->updateFlags &= ~(MOD_UPD_FLG_PLAY_NOTE | MOD_UPD_FLG_SET_VOL);
    break;
  case 0xE:   // Pattern delay
    mod_.patDelay = param;
    break;
  case 0xF:   // Undefined (think of your own use for it!)
    break;
  }
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXSpecialMid(SMODUpdateInfo *vars)
{
  switch(vars->modChn->param >> 4)
  {
  case 0x0:   // Undefined, we use it for callback
    if(mod_.callback != NULL)
      mod_.callback(vars->modChn->param & 0xf, false);
    break;
  case 0x1:   // Fine porta up
    break;
  case 0x2:   // Fine porta down
    break;
  case 0x3:   // Glissando
    break;
  case 0x4:   // Vibrato waveform
    break;
  case 0x5:   // Set finetune
    break;
  case 0x6:   // Pattern loop
    break;
  case 0x7:   // Tremolo waveform
    break;
  case 0x8:   // Set panning (unsupported)
    break;
  case 0x9:   // Retrigger note
    if(--vars->modChn->retrigTick == 0)
    {
      vars->note = vars->modChn->note;
      vars->updateFlags |= MOD_UPD_FLG_PLAY_NOTE;
    }
    break;
  case 0xA:   // Fine volume slide up
    break;
  case 0xB:   // Fine volume slide down
    break;
  case 0xC:   // Note cut
    if(--vars->modChn->noteCutTick == 0)
    {
      vars->modChn->vol = 0;
      vars->updateFlags |= MOD_UPD_FLG_SET_VOL;
      vars->modChn->effect = vars->modChn->param = 0;
    }
    break;
  case 0xD:   // Note delay
    if(--vars->modChn->noteDelayTick == 0)
    {
      vars->note = vars->modChn->note;
      vars->updateFlags |= MOD_UPD_FLG_PLAY_NOTE | MOD_UPD_FLG_SET_VOL;
      vars->modChn->effect = vars->modChn->param = 0;
    }
    break;
  case 0xE:   // Pattern delay
    break;
  case 0xF:   // Undefined (think of your own use for it!)
    break;
  }
}

// -----------------------------------------------------------------------------
void
CMODPlayer::MODFXSpeed(SMODUpdateInfo *vars)
{
  if(vars->param < 32)          //  0- 31 = set speed
    mod_.speed = vars->param;
  else                          // 32-255 = set tempo
    MODSetTempo(vars->param);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
CSoundMixer::CSoundMixer()
{
}

// -----------------------------------------------------------------------------
CSoundMixer::~CSoundMixer()
{
}

// -----------------------------------------------------------------------------
int
CSoundMixer::init()
{
  // Clear channels
  for(int iChannel(0); iChannel < SOUND_CHANNEL_COUNT; iChannel++)
    channel_[iChannel] = NULL;

  return 0;
}

// -----------------------------------------------------------------------------
void
CSoundMixer::setChannel(uint8_t nr, SSoundChannel * data)
{
  if(nr < SOUND_CHANNEL_COUNT)
  {
    channel_[nr] = data;
  }
}

// -----------------------------------------------------------------------------
void
CSoundMixer::mix(uint32_t samplesToMix)
{
  int iActiveChannels(0);  // Count active channels
  int16_t temp[736];

  // Clear temp buffer
  memset(temp, 0, samplesToMix * 2);

  for(int iChannel(0); iChannel < SOUND_CHANNEL_COUNT; iChannel++)
  {
    SSoundChannel * pChannel = channel_[iChannel];

    if((pChannel != NULL) && (pChannel->data != NULL))
    {
      iActiveChannels++;

      if((pChannel->pos + (pChannel->inc * samplesToMix)) >= pChannel->length)
      {
        // Channel will end, so check it
        for(uint32_t i(0); i < samplesToMix; i++)
        {
          temp[i] += pChannel->data[pChannel->pos >> 12] * pChannel->vol;
          pChannel->pos += pChannel->inc;

          if(pChannel->pos >= pChannel->length)
          {
            if(pChannel->loopLength == 0)
            {
              // Stop
              pChannel->data = NULL;
              i = samplesToMix;
            }
            else
            {
              // Loop
              while(pChannel->pos >= pChannel->length)
                pChannel->pos -= pChannel->loopLength;
            }
          }
        }
      }
      else
      {
        // Channel will not end, fast processing
        for(uint32_t i(0); i < samplesToMix; i++)
        {
          temp[i] += pChannel->data[pChannel->pos >> 12] * pChannel->vol;
          pChannel->pos += pChannel->inc;
        }
      }
    }
  }

  // Write temp buffer to output channel
  channelA_.write(temp, samplesToMix);
}
