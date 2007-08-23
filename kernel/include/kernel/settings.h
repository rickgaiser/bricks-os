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

