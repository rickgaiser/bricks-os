#include "kernel/settings.h"
#include "string.h"
#include "iostream"


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
          std::cout<<" - "<<pSettings_[idxSet].name<<" has no value, use AUTO, ON or OFF"<<std::endl;
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
          std::cout<<" - "<<pSettings_[idxSet].name<<" = unknown, use AUTO, ON or OFF"<<std::endl;

        break;
      }
    }
    if(bSettingFound == false)
      std::cout<<"unknown setting "<<cmd<<std::endl;
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

