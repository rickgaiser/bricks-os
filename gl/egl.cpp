#include "EGL/egl.h"


#define EGL_GET_DISPLAY(dpy)      \
{                                 \
  CDisplay * display = &cDisplay; \
}

#define EGL_IF_ERROR(COND, ERRORCODE, RETVAL) \
if(COND)                                      \
{                                             \
  return RETVAL;                              \
}

#define EGL_RETURN(ERRORCODE, RETVAL) \
{                                     \
  return RETVAL;                      \
}

class CDummyDisplay
{
public:
  CDummyDisplay(){}
  virtual ~CDummyDisplay(){}
};

class CDummySurface
{
public:
  CDummySurface(){}
  virtual ~CDummySurface(){}
};

class CDummyContext
{
public:
  CDummyContext(){}
  virtual ~CDummyContext(){}
};

CDummyDisplay cDisplay;
CDummySurface cSurface;
CDummyContext cContext;


//-----------------------------------------------------------------------------
// EGL API
//-----------------------------------------------------------------------------
//EGLAPI EGLint
//EGLAPIENTRY eglGetError(void)
//{
//}

//-----------------------------------------------------------------------------
EGLAPI EGLDisplay
EGLAPIENTRY eglGetDisplay(EGLNativeDisplayType display_id)
{
  EGLDisplay ret = EGL_NO_DISPLAY;
  if(display_id == EGL_DEFAULT_DISPLAY)
    ret = EGL_DEFAULT_DISPLAY;

  EGL_RETURN(EGL_SUCCESS, ret);
}

//-----------------------------------------------------------------------------
EGLAPI EGLBoolean
EGLAPIENTRY eglInitialize(EGLDisplay dpy, EGLint * major, EGLint * minor)
{
  if(major)
    *major = 1;
  if(minor)
    *minor = 2;

  EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

//-----------------------------------------------------------------------------
EGLAPI EGLBoolean
EGLAPIENTRY eglTerminate(EGLDisplay dpy)
{
  EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

//-----------------------------------------------------------------------------
//EGLAPI const char *
//EGLAPIENTRY eglQueryString(EGLDisplay dpy, EGLint name)
//{
//}

//-----------------------------------------------------------------------------
//EGLAPI EGLBoolean
//EGLAPIENTRY eglGetConfigs(EGLDisplay dpy, EGLConfig * configs, EGLint config_size, EGLint * num_config)
//{
//}

//-----------------------------------------------------------------------------
EGLAPI EGLBoolean
EGLAPIENTRY eglChooseConfig(EGLDisplay dpy, const EGLint * attrib_list, EGLConfig * configs, EGLint config_size, EGLint * num_config)
{
  EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

//-----------------------------------------------------------------------------
//EGLAPI EGLBoolean
//EGLAPIENTRY eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint * value)
//{
//}

//-----------------------------------------------------------------------------
EGLAPI EGLSurface
EGLAPIENTRY eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, EGLNativeWindowType win, const EGLint * attrib_list)
{
  EGL_RETURN(EGL_SUCCESS, (EGLSurface)&cSurface);
}

//-----------------------------------------------------------------------------
//EGLAPI EGLSurface
//EGLAPIENTRY eglCreatePbufferSurface(EGLDisplay dpy, EGLConfig config, const EGLint * attrib_list)
//{
//}

//-----------------------------------------------------------------------------
//EGLAPI EGLSurface
//EGLAPIENTRY eglCreatePixmapSurface(EGLDisplay dpy, EGLConfig config, EGLNativePixmapType pixmap, const EGLint * attrib_list)
//{
//}

//-----------------------------------------------------------------------------
EGLAPI EGLBoolean
EGLAPIENTRY eglDestroySurface(EGLDisplay dpy, EGLSurface surface)
{
  EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

//-----------------------------------------------------------------------------
//EGLAPI EGLBoolean
//EGLAPIENTRY eglQuerySurface(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint * value)
//{
//}

//-----------------------------------------------------------------------------
EGLAPI EGLBoolean
EGLAPIENTRY eglBindAPI(EGLenum api)
{
  if(api != EGL_OPENGL_ES_API)
  {
    EGL_RETURN(EGL_BAD_PARAMETER, EGL_FALSE);
  }
  else
  {
    EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
  }
}

//-----------------------------------------------------------------------------
//EGLAPI EGLenum
//EGLAPIENTRY eglQueryAPI(void)
//{
//}

//-----------------------------------------------------------------------------
//EGLAPI EGLBoolean
//EGLAPIENTRY eglWaitClient(void)
//{
//}

//-----------------------------------------------------------------------------
//EGLAPI EGLBoolean
//EGLAPIENTRY eglReleaseThread(void)
//{
//}

//-----------------------------------------------------------------------------
//EGLAPI EGLSurface
//EGLAPIENTRY eglCreatePbufferFromClientBuffer(EGLDisplay dpy, EGLenum buftype, EGLClientBuffer buffer, EGLConfig config, const EGLint * attrib_list)
//{
//}

//-----------------------------------------------------------------------------
//EGLAPI EGLBoolean
//EGLAPIENTRY eglSurfaceAttrib(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint value)
//{
//}

//-----------------------------------------------------------------------------
//EGLAPI EGLBoolean
//EGLAPIENTRY eglBindTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
//{
//}

//-----------------------------------------------------------------------------
//EGLAPI EGLBoolean
//EGLAPIENTRY eglReleaseTexImage(EGLDisplay dpy, EGLSurface surface, EGLint buffer)
//{
//}

//-----------------------------------------------------------------------------
//EGLAPI EGLBoolean
//EGLAPIENTRY eglSwapInterval(EGLDisplay dpy, EGLint interval)
//{
//}

//-----------------------------------------------------------------------------
EGLAPI EGLContext
EGLAPIENTRY eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint * attrib_list)
{
  EGL_RETURN(EGL_SUCCESS, (EGLContext)&cContext);
}

//-----------------------------------------------------------------------------
EGLAPI EGLBoolean
EGLAPIENTRY eglDestroyContext(EGLDisplay dpy, EGLContext ctx)
{
  EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

//-----------------------------------------------------------------------------
EGLAPI EGLBoolean
EGLAPIENTRY eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
  EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

//-----------------------------------------------------------------------------
//EGLAPI EGLContext
//EGLAPIENTRY eglGetCurrentContext(void)
//{
//}

//-----------------------------------------------------------------------------
//EGLAPI EGLSurface
//EGLAPIENTRY eglGetCurrentSurface(EGLint readdraw)
//{
//}

//-----------------------------------------------------------------------------
//EGLAPI EGLDisplay
//EGLAPIENTRY eglGetCurrentDisplay(void)
//{
//}

//-----------------------------------------------------------------------------
//EGLAPI EGLBoolean
//EGLAPIENTRY eglQueryContext(EGLDisplay dpy, EGLContext ctx, EGLint attribute, EGLint * value)
//{
//}

//-----------------------------------------------------------------------------
//EGLAPI EGLBoolean
//EGLAPIENTRY eglWaitGL(void)
//{
//}

//-----------------------------------------------------------------------------
//EGLAPI EGLBoolean
//EGLAPIENTRY eglWaitNative(EGLint engine)
//{
//}

//-----------------------------------------------------------------------------
EGLAPI EGLBoolean
EGLAPIENTRY eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
  EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

//-----------------------------------------------------------------------------
//EGLAPI EGLBoolean
//EGLAPIENTRY eglCopyBuffers(EGLDisplay dpy, EGLSurface surface, EGLNativePixmapType target)
//{
//}

//-----------------------------------------------------------------------------
//EGLAPI void
//(* EGLAPIENTRY eglGetProcAddress(const char * procname))(void)
//{
//}
