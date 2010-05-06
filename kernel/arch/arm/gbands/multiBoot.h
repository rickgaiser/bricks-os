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


#ifndef GBANDS_MULTIBOOT_H
#define GBANDS_MULTIBOOT_H


#include "inttypes.h"


struct MultiBootParam
{
  uint32_t   reserved1[5];
  uint8_t    handshake_data;
  uint8_t    padding;
  uint16_t   handshake_timeout;
  uint8_t    probe_count;
  uint8_t    client_data[3];
  uint8_t    palette_data;
  uint8_t    response_bit;
  uint8_t    client_bit;
  uint8_t    reserved2;
  uint8_t  * boot_srcp;
  uint8_t  * boot_endp;
  uint8_t  * masterp;
  uint8_t  * reserved3[3];
  uint32_t   system_work2[4];
  uint8_t    sendflag;
  uint8_t    probe_target_bit;
  uint8_t    check_wait;
  uint8_t    server_type;
};

enum MULTIBOOT_MODES
{
  MODE32_NORMAL = 0,
  MODE16_MULTI  = 1,
  MODE32_2MHZ   = 2
};


#endif
