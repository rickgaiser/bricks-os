#ifndef STDDEF_H
#define STDDEF_H


#ifdef __cplusplus
extern "C" {
#endif


#ifndef NULL
#define NULL 0
#endif


#ifndef __WCHAR_TYPE__
#define __WCHAR_TYPE__ int
#endif
#ifndef __cplusplus
typedef __WCHAR_TYPE__ wchar_t;
#endif

typedef unsigned int size_t;


#ifdef __cplusplus
}
#endif


#endif
