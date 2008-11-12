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


#include "kernel/debug.h"
#include "kernel/settings.h"
#include "string.h"


// -----------------------------------------------------------------------------
CSettings::CSettings(SSetting * settings, unsigned int size)
 : pSettings_(settings)
 , iSize_(size)
{
}

// -----------------------------------------------------------------------------
void
CSettings::parse(const char * cmd)
{
  while(true)
  {
    // Advande to next ' '
    while(*cmd != ' ' && *cmd != 0)
      cmd++;

    // Advance to next non ' ' character
    while(*cmd == ' ' && *cmd != 0)
      cmd++;

    // Validate character
    if(*cmd == 0)
      break;

    // Compare argument to settings list
    bool bSettingFound(false);
    for(unsigned int idxSet(0); idxSet < iSize_; idxSet++)
    {
      if(cmp(pSettings_[idxSet].name, cmd) == true)
      {
        bSettingFound = true;

        // Advance to next '='
        while(*cmd != '=' && *cmd != 0)
          cmd++;

        // Validate character
        if((cmd[0] == 0) || (cmd[1] == 0))
        {
          printk(" - %d has no value, use AUTO, ON or OFF\n", pSettings_[idxSet].name);
          break;
        }
        cmd++;

        if(cmp(cmd, "AUTO") == true)
          pSettings_[idxSet].value = SET_AUTO;
        else if(cmp(cmd, "ON") == true)
          pSettings_[idxSet].value = SET_ON;
        else if(cmp(cmd, "OFF") == true)
          pSettings_[idxSet].value = SET_OFF;
        else
          printk(" - %d = unknown, use AUTO, ON or OFF\n", pSettings_[idxSet].name);

        break;
      }
    }
    if(bSettingFound == false)
      printk("unknown setting %d\n", cmd);
  }
}

// -----------------------------------------------------------------------------
int
CSettings::get(const char * name)
{
  for(unsigned int i(0); i < iSize_; i++)
    if(strcmp(name, pSettings_[i].name) == 0)
      return pSettings_[i].value;
  return -1;
}

// -----------------------------------------------------------------------------
bool
CSettings::cmp(const char * s1, const char * s2)
{
  for(int i(0); s1[i] == s2[i]; i++)
    if((s1[i + 1] == 0) || (s2[i + 1] == 0))
      return true;
  return false;
}

