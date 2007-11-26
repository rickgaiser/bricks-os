#ifndef STDDEF_H
#define STDDEF_H


#ifndef NULL
#define NULL (0)
#endif


#ifndef __WCHAR_TYPE__
#define __WCHAR_TYPE__ int
#endif
#ifndef __cplusplus
typedef __WCHAR_TYPE__ wchar_t;
#endif

typedef long ptrdiff_t;
#ifndef __SIZE_T__
#define __SIZE_T__
typedef unsigned int size_t;
#endif


#endif
