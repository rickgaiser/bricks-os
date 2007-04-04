/* Platform-specific types and definitions for egl.h */
#ifndef __eglplatform_h_
#define __eglplatform_h_


#include "inttypes.h"


#ifndef EGLAPIENTRY
#define EGLAPIENTRY
#endif
#ifndef EGLAPI
#define EGLAPI extern
#endif


typedef void * NativeDisplayType;
typedef void * NativePixmapType;
typedef void * NativeWindowType;

/* EGL 1.2 types, renamed for consistency in EGL 1.3 */
typedef NativeDisplayType EGLNativeDisplayType;
typedef NativePixmapType EGLNativePixmapType;
typedef NativeWindowType EGLNativeWindowType;


#endif /* __eglplatform_h */
