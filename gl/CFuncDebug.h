#ifndef CFUNCDEBUG_H
#define CFUNCDEBUG_H


class CFuncDebug
{
public:
  CFuncDebug(const char * sFunc);
  ~CFuncDebug();

private:
  static int iIndent_;
  const char * sFunc_;
};


// Always print
#define FUNCTION_DEBUG0()    CFuncDebug cFuncDebug(__func__);

// General functions
//#define FUNCTION_DEBUG()    FUNCTION_DEBUG0()
#define FUNCTION_DEBUG()

// Inner rendering loops: color/vertex/texcoord/normal
//#define FUNCTION_DEBUG2()    FUNCTION_DEBUG0()
#define FUNCTION_DEBUG2()

// Inner rendering loops: begin/end/flush
//#define FUNCTION_DEBUG3()    FUNCTION_DEBUG0()
#define FUNCTION_DEBUG3()


#endif
