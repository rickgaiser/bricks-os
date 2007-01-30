#ifndef CPU_H
#define CPU_H


class CCPU
{
public:
  static inline void cli(){}
  static inline void sti(){}
  static inline void halt(){while(true){}}

private:
  CCPU(){}
};


#endif
