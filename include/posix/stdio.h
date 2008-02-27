#ifndef STDIO_H
#define STDIO_H


#ifndef NULL
#define NULL (0)
#endif

#ifndef EOF
#define EOF (-1)
#endif


typedef struct FILE
{
  int fd;
} FILE;


int printf(const char * format, ...);


extern FILE * stdin;
extern FILE * stdout;
extern FILE * stderr;


#endif
