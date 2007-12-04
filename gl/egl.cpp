#include "EGL/egl.h"
#include "context.h"
#include "stddef.h"


extern IGLESContext * getGLESContext();


#define EGL_GET_THREAD() \
CEGLThread * thread = &cThread; \

#define EGL_GET_DISPLAY(dpy) \
CEGLDisplay * display = pDisplay; \

#define EGL_IF_ERROR(COND, ERRORCODE, RETVAL) \
if(COND)                                      \
{                                             \
  iError = ERRORCODE;                         \
  return RETVAL;                              \
}

#define EGL_RETURN(ERRORCODE, RETVAL) \
{                                     \
  iError = ERRORCODE;                 \
  return RETVAL;                      \
}

//-----------------------------------------------------------------------------
class CRefCount
{
public:
  CRefCount() : iRefCount_(0){}
  virtual ~CRefCount(){}

  void addReference()   { iRefCount_++; }
  int  removeReference(){ iRefCount_--; if(iRefCount_ < 0)iRefCount_ = 0; return iRefCount_; }

private:
  int iRefCount_;
};

//-----------------------------------------------------------------------------
class CEGLSurface
 : public CRefCount
{
public:
  CEGLSurface() : CRefCount(){}
  virtual ~CEGLSurface(){}

  CSurface * pNativeSurface_;
};

//-----------------------------------------------------------------------------
class CEGLContext
 : public CRefCount
{
public:
  CEGLContext() : CRefCount(), pGLESContext_(NULL){}
  virtual ~CEGLContext(){}

  IGLESContext * pGLESContext_;
};

//-----------------------------------------------------------------------------
class CEGLDisplay
{
public:
  CEGLDisplay() : pContext_(NULL), pSurface_(NULL){}
  virtual ~CEGLDisplay(){}

  CEGLContext * pContext_; // FIXME: Should be a list
  CEGLSurface * pSurface_; // FIXME: Should be a list
};

//-----------------------------------------------------------------------------
class CEGLThread
{
public:
  CEGLThread() : pContext_(NULL), pSurface_(NULL){}
  virtual ~CEGLThread(){}

  CEGLContext * pContext_;
  CEGLSurface * pSurface_;
  EGLenum       api_;
};


CEGLDisplay * pDisplay = NULL;
CEGLThread    cThread;
EGLint        iError(EGL_SUCCESS);


//-----------------------------------------------------------------------------
// EGL API
//-----------------------------------------------------------------------------
EGLAPI EGLint
EGLAPIENTRY eglGetError(void)
{
  return iError;
}

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
  EGL_GET_DISPLAY(dpy);
  EGL_IF_ERROR(display, EGL_SUCCESS, EGL_TRUE); // Already initialized
  EGL_IF_ERROR(dpy != EGL_DEFAULT_DISPLAY, EGL_BAD_DISPLAY, EGL_FALSE);

  // Allocate display
  pDisplay = new CEGLDisplay;
  if(pDisplay == NULL)
    EGL_RETURN(EGL_BAD_ALLOC, EGL_FALSE);

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
  EGL_GET_DISPLAY(dpy);
  EGL_IF_ERROR(!display, EGL_SUCCESS, EGL_TRUE); // Already terminated
  EGL_IF_ERROR(dpy != EGL_DEFAULT_DISPLAY, EGL_BAD_DISPLAY, EGL_FALSE);

  // Remove references to contexts
  if((pDisplay->pContext_ != NULL) && (pDisplay->pContext_->removeReference() == 0))
    delete pDisplay->pContext_;

  // Remove references to surfaces
  if((pDisplay->pSurface_ != NULL) && (pDisplay->pSurface_->removeReference() == 0))
    delete pDisplay->pSurface_;

  // Delete display
  if(pDisplay != NULL)
    delete pDisplay;

  EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

//-----------------------------------------------------------------------------
EGLAPI const char *
EGLAPIENTRY eglQueryString(EGLDisplay dpy, EGLint name)
{
  const char * ret(0);
  static const char apis[] = "OpenGL_ES";
  static const char extensions[] = "";
  static const char vendor[] = "Bricks-OS";
  static const char version[] = "1.2";

  switch(name)
  {
  case EGL_CLIENT_APIS:
    ret = apis;
    break;
  case EGL_EXTENSIONS:
    ret = extensions;
    break;
  case EGL_VENDOR:
    ret = vendor;
    break;
  case EGL_VERSION:
    ret = version;
    break;
  default:
    EGL_RETURN(EGL_BAD_PARAMETER, 0);
  }

  EGL_RETURN(EGL_SUCCESS, ret);
}

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
  EGL_GET_DISPLAY(dpy);
  EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_NO_SURFACE);
  EGL_IF_ERROR(dpy != EGL_DEFAULT_DISPLAY, EGL_BAD_DISPLAY, EGL_NO_SURFACE);
//  EGL_IF_ERROR(!eglConfigExists(display, config), EGL_BAD_CONFIG, EGL_NO_SURFACE);

  // FIXME: Only one surface supported
  if(display->pSurface_ != NULL)
    EGL_RETURN(EGL_BAD_ALLOC, EGL_NO_SURFACE);

  // Allocate new surface
  display->pSurface_ = new CEGLSurface;
  if(display->pSurface_ == NULL)
    EGL_RETURN(EGL_BAD_ALLOC, EGL_NO_SURFACE);

  display->pSurface_->addReference();
  display->pSurface_->pNativeSurface_ = (CSurface *)win;

  EGL_RETURN(EGL_SUCCESS, (EGLSurface)display->pSurface_);
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
  EGL_GET_DISPLAY(dpy);
  EGL_IF_ERROR(!display, EGL_NOT_INITIALIZED, EGL_FALSE);
  EGL_IF_ERROR(dpy != EGL_DEFAULT_DISPLAY, EGL_BAD_DISPLAY, EGL_FALSE);
//  EGL_IF_ERROR(!eglSurfaceExists(display, surface), EGL_BAD_SURFACE, EGL_FALSE);

  // Remove reference
  if(((CEGLSurface *)surface)->removeReference() == 0)
    delete ((CEGLSurface *)surface);

  display->pSurface_ = NULL;

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
  EGL_GET_THREAD();

  if(api == EGL_OPENGL_ES_API)
  {
    thread->api_ = api;
    EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
  }
  else
  {
    EGL_RETURN(EGL_BAD_PARAMETER, EGL_FALSE);
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
  CEGLContext * pNewContext  = new CEGLContext;
  pNewContext->pGLESContext_ = getGLESContext();

  EGL_RETURN(EGL_SUCCESS, (EGLContext)pNewContext);
}

//-----------------------------------------------------------------------------
EGLAPI EGLBoolean
EGLAPIENTRY eglDestroyContext(EGLDisplay dpy, EGLContext ctx)
{
  delete ((CEGLContext *)ctx)->pGLESContext_;
  delete ((CEGLContext *)ctx);

  EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

//-----------------------------------------------------------------------------
EGLAPI EGLBoolean
EGLAPIENTRY eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx)
{
  EGL_GET_THREAD();

  thread->pContext_ = (CEGLContext *)ctx;
  thread->pSurface_ = (CEGLSurface *)draw;

  if((thread->pContext_ != NULL) &&
     (thread->pContext_->pGLESContext_ != NULL) &&
     (thread->pSurface_ != NULL) &&
     (thread->pSurface_->pNativeSurface_ != NULL))
  {
    thread->pContext_->pGLESContext_->setSurface(thread->pSurface_->pNativeSurface_);
  }

  EGL_RETURN(EGL_SUCCESS, EGL_TRUE);
}

//-----------------------------------------------------------------------------
EGLAPI EGLContext
EGLAPIENTRY eglGetCurrentContext(void)
{
  EGL_GET_THREAD();

  EGL_RETURN(EGL_SUCCESS, (EGLContext)thread->pContext_);
}

//-----------------------------------------------------------------------------
EGLAPI EGLSurface
EGLAPIENTRY eglGetCurrentSurface(EGLint readdraw)
{
  EGL_GET_THREAD();

  EGL_RETURN(EGL_SUCCESS, (EGLSurface)thread->pSurface_);
}

//-----------------------------------------------------------------------------
EGLAPI EGLDisplay
EGLAPIENTRY eglGetCurrentDisplay(void)
{
  EGL_GET_DISPLAY(EGL_DEFAULT_DISPLAY);

  EGL_RETURN(EGL_SUCCESS, display);
}

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

//-----------------------------------------------------------------------------
void *
eglGetCurrentGLESContext()
{
  EGL_GET_THREAD();

  return (void *)thread->pContext_->pGLESContext_;
}
