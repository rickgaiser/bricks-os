#ifndef FILESTREAM_H
#define FILESTREAM_H


#include "ace/Task.h"


class CFileStream
 : public ACE_Task_Base
{
public:
  CFileStream(const char * in, const char * out);
  ~CFileStream();

  bool start();

  int svc();

private:
  const char * pIn_;
  const char * pOut_;
  int fdIn_;
  int fdOut_;
};


#endif
