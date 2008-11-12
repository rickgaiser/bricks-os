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
 */


#ifndef SETTINGS_H
#define SETTINGS_H


#define SET_AUTO  0x00
#define SET_ON    0x01
#define SET_OFF   0x02


struct SSetting
{
  const char * name;
  int value;
};

class CSettings
{
public:
  CSettings(SSetting * settings, unsigned int size);

  void parse(const char * cmd);
  int get(const char * name);

private:
  bool cmp(const char * s1, const char * s2);

  SSetting * pSettings_;
  unsigned int iSize_;
};


#endif

