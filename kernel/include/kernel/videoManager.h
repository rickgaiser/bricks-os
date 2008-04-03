#ifndef KERNEL_VIDEOMANAGER_H
#define KERNEL_VIDEOMANAGER_H


#include "inttypes.h"
#include "GL/gl.h"


//---------------------------------------------------------------------------
// Main color format type, can hold any color <= 32 bits
typedef uint32_t color_t;
// Color struct
struct SColor
{
  uint8_t r, g, b, a;
};

//---------------------------------------------------------------------------
struct SColorFormatOperations
{
  uint8_t  bitsPerPixel;
  uint8_t  lossR;
  uint8_t  lossG;
  uint8_t  lossB;
  uint8_t  lossA;
  uint8_t  shiftR;
  uint8_t  shiftG;
  uint8_t  shiftB;
  uint8_t  shiftA;
  uint32_t maskR;
  uint32_t maskG;
  uint32_t maskB;
  uint32_t maskA;
};
extern const SColorFormatOperations colorFormatOps[];

//---------------------------------------------------------------------------
enum EColorFormat
{
    cfUNKNOWN       = 0
    // (A/X)RGB (32bit)
  , cfR8G8B8        = 1
  , cfA8R8G8B8      = 2
  , cfX8R8G8B8      = 3
    // (A/X)BGR (32bit)
  , cfB8G8R8        = 4
  , cfA8B8G8R8      = 5
  , cfX8B8G8R8      = 6
    // (A/X)RGB (16bit)
  , cfR5G6B5        = 7
  , cfA1R5G5B5      = 8
  , cfX1R5G5B5      = 9
  , cfA4R4G4B4      = 10
  , cfX4R4G4B4      = 11
    // (A/X)BGR (16bit)
  , cfB5G6R5        = 12
  , cfA1B5G5R5      = 13  // NDS
  , cfX1B5G5R5      = 14  // GBA
  , cfA4B4G4R4      = 15
  , cfX4B4G4R4      = 16
    // Palettized RGB (8bit)
  , cfR3G3B2        = 17
    // Palettized
  , cfP8            = 18  // 256 colors (8 bit)
  , cfP4            = 19  //  16 colors (4 bit)
};

//---------------------------------------------------------------------------
// Create color formats
#define BxColorFormat_FromRGBA(fmt,r,g,b,a) \
  ((r >> colorFormatOps[fmt].lossR) << colorFormatOps[fmt].shiftR) | \
  ((g >> colorFormatOps[fmt].lossG) << colorFormatOps[fmt].shiftG) | \
  ((b >> colorFormatOps[fmt].lossB) << colorFormatOps[fmt].shiftB) | \
  ((a >> colorFormatOps[fmt].lossA) << colorFormatOps[fmt].shiftA)
#define BxColorFormat_FromRGB(fmt,r,g,b) \
  ((r >> colorFormatOps[fmt].lossR) << colorFormatOps[fmt].shiftR) | \
  ((g >> colorFormatOps[fmt].lossG) << colorFormatOps[fmt].shiftG) | \
  ((b >> colorFormatOps[fmt].lossB) << colorFormatOps[fmt].shiftB) | \
  (colorFormatOps[fmt].maskA)
// Get colors from color formats
#define BxColorFormat_GetR(fmt,color) \
  (((color & colorFormatOps[fmt].maskR) >> colorFormatOps[fmt].shiftR) << colorFormatOps[fmt].lossR)
#define BxColorFormat_GetG(fmt,color) \
  (((color & colorFormatOps[fmt].maskG) >> colorFormatOps[fmt].shiftG) << colorFormatOps[fmt].lossG)
#define BxColorFormat_GetB(fmt,color) \
  (((color & colorFormatOps[fmt].maskB) >> colorFormatOps[fmt].shiftB) << colorFormatOps[fmt].lossB)
#define BxColorFormat_GetA(fmt,color) \
  (((color & colorFormatOps[fmt].maskA) >> colorFormatOps[fmt].shiftA) << colorFormatOps[fmt].lossA)
// Convert color formats
#define BxColorFormat_Convert(fmtFrom,fmtTo,color) \
  BxColorFormat_FromRGBA(fmtTo, \
                         BxColorFormat_GetR(fmtFrom, color), \
                         BxColorFormat_GetG(fmtFrom, color), \
                         BxColorFormat_GetB(fmtFrom, color), \
                         BxColorFormat_GetA(fmtFrom, color))

//---------------------------------------------------------------------------
enum ESurfaceType
{
    stSCREEN
  , stOFFSCREEN
};

//---------------------------------------------------------------------------
// Video Modes
#define VM_PROGRESSIVE      0x00000001 // Full Frames, best quality
#define VM_INTERLACED       0x00000002 // Very efficient when double buffering, but keep up the framerate!
#define VM_FULL_INTERLACED  0x00000004 // Uses Full Frames (good for double buffering when framerate can't keep up)

//---------------------------------------------------------------------------
typedef struct SVideoMode
{
  uint32_t xpitch;           // Framebuffer width
  uint32_t ypitch;           // Framebuffer height
  uint32_t width;            // Visible width
  uint32_t height;           // Visible height
  uint32_t bpp;              // Bits per pixel
//  uint16_t aRatioW;          // Aspect Ratio Width
//  uint16_t aRatioH;          // Aspect Ratio Height
  EColorFormat format;       // Pixel format
//  uint32_t flags;
} SVideoMode;

//---------------------------------------------------------------------------
class CSurface
{
public:
  CSurface();
  virtual ~CSurface();

  // Video mode for surface
  virtual uint32_t xpitch();
  virtual uint32_t ypitch();
  virtual uint32_t width();
  virtual uint32_t height();
  virtual uint32_t bpp();
  virtual EColorFormat format();

//protected:
public:
  // Data
  void * p;

  // Video mode for surface
  SVideoMode mode;
};

//---------------------------------------------------------------------------
class IRenderer
{
public:
  virtual ~IRenderer(){}

  // Surfaces
  virtual void       setSurface(CSurface * surface) = 0;
  virtual CSurface * getSurface() = 0;

  // Flush operations to surface
  virtual void       flush() = 0;
};

//---------------------------------------------------------------------------
class CASoftwareRenderer
 : public virtual IRenderer
{
public:
  virtual ~CASoftwareRenderer(){}

  // Surfaces
  virtual void       setSurface(CSurface * surface){renderSurface = surface;}
  virtual CSurface * getSurface()                  {return renderSurface;}

  // Flush operations to surface
  virtual void       flush()                       {}

protected:
  CSurface  * renderSurface;
};

//---------------------------------------------------------------------------
class I2DRenderer
 : public virtual IRenderer
{
public:
  virtual ~I2DRenderer(){}

  // Surfaces
  virtual void       setSurface(CSurface * surface) = 0;
  virtual CSurface * getSurface() = 0;

  // Flush operations to surface
  virtual void       flush() = 0;

  // Color
  virtual void       setColor(color_t rgb) = 0; // cfA8R8G8B8 format color
  virtual void       setColor(uint8_t r, uint8_t g, uint8_t b) = 0;

  // Drawing
  virtual void       setPixel(int x, int y) = 0;
  virtual void       fill() = 0;
  virtual void       fillRect(int x, int y, unsigned int width, unsigned int height) = 0;  // Width: width in pixels, Height: height in pixels
  virtual void       drawLine(int x1, int y1, int x2, int y2) = 0;                         // Note: x2 and y2 are drawn
  virtual void       drawHLine(int x, int y, unsigned int width) = 0;
  virtual void       drawVLine(int x, int y, unsigned int height) = 0;
  virtual void       drawRect(int x, int y, unsigned int width, unsigned int height) = 0;
};

//-----------------------------------------------------------------------------
class I3DRenderer
 : public virtual IRenderer
{
public:
  virtual ~I3DRenderer(){}

  // Surfaces
  virtual void       setSurface(CSurface * surface) = 0;
  virtual CSurface * getSurface() = 0;

  // Flush operations to surface
  virtual void       flush() = 0;

//  virtual void glAlphaFunc(GLenum func, GLclampf ref) = 0;
  virtual void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha) = 0;
  virtual void glClearDepthf(GLclampf depth) = 0;
//  virtual void glClipPlanef(GLenum plane, const GLfloat *equation) = 0;
  virtual void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha) = 0;
  virtual void glDepthRangef(GLclampf zNear, GLclampf zFar) = 0;
  virtual void glFogf(GLenum pname, GLfloat param) = 0;
  virtual void glFogfv(GLenum pname, const GLfloat *params) = 0;
  virtual void glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) = 0;
//  virtual void glGetClipPlanef(GLenum pname, GLfloat eqn[4]) = 0;
//  virtual void glGetFloatv(GLenum pname, GLfloat *params) = 0;
//  virtual void glGetLightfv(GLenum light, GLenum pname, GLfloat *params) = 0;
//  virtual void glGetMaterialfv(GLenum face, GLenum pname, GLfloat *params) = 0;
//  virtual void glGetTexEnvfv(GLenum env, GLenum pname, GLfloat *params) = 0;
//  virtual void glGetTexParameterfv(GLenum target, GLenum pname, GLfloat *params) = 0;
//  virtual void glLightModelf(GLenum pname, GLfloat param) = 0;
//  virtual void glLightModelfv(GLenum pname, const GLfloat *params) = 0;
  virtual void glLightf(GLenum light, GLenum pname, GLfloat param) = 0;
  virtual void glLightfv(GLenum light, GLenum pname, const GLfloat * params) = 0;
//  virtual void glLineWidth(GLfloat width) = 0;
  virtual void glLoadMatrixf(const GLfloat *m) = 0;
//  virtual void glMaterialf(GLenum face, GLenum pname, GLfloat param) = 0;
//  virtual void glMaterialfv(GLenum face, GLenum pname, const GLfloat *params) = 0;
  virtual void glMultMatrixf(const GLfloat *m) = 0;
//  virtual void glMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q) = 0;
  virtual void glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz) = 0;
  virtual void glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) = 0;
//  virtual void glPointParameterf(GLenum pname, GLfloat param) = 0;
//  virtual void glPointParameterfv(GLenum pname, const GLfloat *params) = 0;
//  virtual void glPointSize(GLfloat size) = 0;
//  virtual void glPolygonOffset(GLfloat factor, GLfloat units) = 0;
  virtual void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) = 0;
  virtual void glScalef(GLfloat x, GLfloat y, GLfloat z) = 0;
//  virtual void glTexEnvf(GLenum target, GLenum pname, GLfloat param) = 0;
//  virtual void glTexEnvfv(GLenum target, GLenum pname, const GLfloat *params) = 0;
  virtual void glTexParameterf(GLenum target, GLenum pname, GLfloat param) = 0;
//  virtual void glTexParameterfv(GLenum target, GLenum pname, const GLfloat *params) = 0;
  virtual void glTranslatef(GLfloat x, GLfloat y, GLfloat z) = 0;

//  virtual void glActiveTexture(GLenum texture) = 0;
//  virtual void glAlphaFuncx(GLenum func, GLclampx ref) = 0;
//  virtual void glBindBuffer(GLenum target, GLuint buffer) = 0;
  virtual void glBindTexture(GLenum target, GLuint texture) = 0;
//  virtual void glBlendFunc(GLenum sfactor, GLenum dfactor) = 0;
//  virtual void glBufferData(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage) = 0;
//  virtual void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data) = 0;
  virtual void glClear(GLbitfield mask) = 0;
  virtual void glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha) = 0;
  virtual void glClearDepthx(GLclampx depth) = 0;
//  virtual void glClearStencil(GLint s) = 0;
//  virtual void glClientActiveTexture(GLenum texture) = 0;
//  virtual void glClipPlanex(GLenum plane, const GLfixed *equation) = 0;
  virtual void glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha) = 0;
  virtual void glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha) = 0;
//  virtual void glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha) = 0;
  virtual void glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer) = 0;
//  virtual void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data) = 0;
//  virtual void glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data) = 0;
//  virtual void glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border) = 0;
//  virtual void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height) = 0;
  virtual void glCullFace(GLenum mode) = 0;
//  virtual void glDeleteBuffers(GLsizei n, const GLuint *buffers) = 0;
  virtual void glDeleteTextures(GLsizei n, const GLuint *textures) = 0;
  virtual void glDepthFunc(GLenum func) = 0;
//  virtual void glDepthMask(GLboolean flag) = 0;
  virtual void glDepthRangex(GLclampx zNear, GLclampx zFar) = 0;
  virtual void glDisable(GLenum cap) = 0;
  virtual void glDisableClientState(GLenum array) = 0;
  virtual void glDrawArrays(GLenum mode, GLint first, GLsizei count) = 0;
//  virtual void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices) = 0;
  virtual void glEnable(GLenum cap) = 0;
  virtual void glEnableClientState(GLenum array) = 0;
  virtual void glFinish(void) = 0;
  virtual void glFlush(void) = 0;
  virtual void glFogx(GLenum pname, GLfixed param) = 0;
  virtual void glFogxv(GLenum pname, const GLfixed *params) = 0;
  virtual void glFrontFace(GLenum mode) = 0;
  virtual void glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar) = 0;
//  virtual void glGetBooleanv(GLenum pname, GLboolean *params) = 0;
//  virtual void glGetBufferParameteriv(GLenum target, GLenum pname, GLint *params) = 0;
//  virtual void glGetClipPlanex(GLenum pname, GLfixed eqn[4]) = 0;
//  virtual void glGenBuffers(GLsizei n, GLuint *buffers) = 0;
  virtual void glGenTextures(GLsizei n, GLuint *textures) = 0;
//  virtual GLenum glGetError(void) = 0;
//  virtual void glGetFixedv(GLenum pname, GLfixed *params) = 0;
//  virtual void glGetIntegerv(GLenum pname, GLint *params) = 0;
//  virtual void glGetLightxv(GLenum light, GLenum pname, GLfixed *params) = 0;
//  virtual void glGetMaterialxv(GLenum face, GLenum pname, GLfixed *params) = 0;
//  virtual void glGetPointerv(GLenum pname, void **params) = 0;
//  virtual const GLubyte * glGetString(GLenum name) = 0;
//  virtual void glGetTexEnviv(GLenum env, GLenum pname, GLint *params) = 0;
//  virtual void glGetTexEnvxv(GLenum env, GLenum pname, GLfixed *params) = 0;
//  virtual void glGetTexParameteriv(GLenum target, GLenum pname, GLint *params) = 0;
//  virtual void glGetTexParameterxv(GLenum target, GLenum pname, GLfixed *params) = 0;
//  virtual void glHint(GLenum target, GLenum mode) = 0;
//  virtual GLboolean glIsBuffer(GLuint buffer) = 0;
//  virtual GLboolean glIsEnabled(GLenum cap) = 0;
//  virtual GLboolean glIsTexture(GLuint texture) = 0;
//  virtual void glLightModelx(GLenum pname, GLfixed param) = 0;
//  virtual void glLightModelxv(GLenum pname, const GLfixed *params) = 0;
  virtual void glLightx(GLenum light, GLenum pname, GLfixed param) = 0;
  virtual void glLightxv(GLenum light, GLenum pname, const GLfixed * params) = 0;
//  virtual void glLineWidthx(GLfixed width) = 0;
  virtual void glLoadIdentity(void) = 0;
  virtual void glLoadMatrixx(const GLfixed *m) = 0;
//  virtual void glLogicOp(GLenum opcode) = 0;
//  virtual void glMaterialx(GLenum face, GLenum pname, GLfixed param) = 0;
//  virtual void glMaterialxv(GLenum face, GLenum pname, const GLfixed *params) = 0;
  virtual void glMatrixMode(GLenum mode) = 0;
  virtual void glMultMatrixx(const GLfixed *m) = 0;
//  virtual void glMultiTexCoord4x(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q) = 0;
  virtual void glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz) = 0;
  virtual void glNormalPointer(GLenum type, GLsizei stride, const GLvoid * pointer) = 0;
  virtual void glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar) = 0;
//  virtual void glPixelStorei(GLenum pname, GLint param) = 0;
//  virtual void glPointParameterx(GLenum pname, GLfixed param) = 0;
//  virtual void glPointParameterxv(GLenum pname, const GLfixed *params) = 0;
//  virtual void glPointSizex(GLfixed size) = 0;
//  virtual void glPolygonOffsetx(GLfixed factor, GLfixed units) = 0;
//  virtual void glPopMatrix(void) = 0;
//  virtual void glPushMatrix(void) = 0;
//  virtual void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels) = 0;
  virtual void glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z) = 0;
//  virtual void glSampleCoverage(GLclampf value, GLboolean invert) = 0;
//  virtual void glSampleCoveragex(GLclampx value, GLboolean invert) = 0;
  virtual void glScalex(GLfixed x, GLfixed y, GLfixed z) = 0;
//  virtual void glScissor(GLint x, GLint y, GLsizei width, GLsizei height) = 0;
  virtual void glShadeModel(GLenum mode) = 0;
//  virtual void glStencilFunc(GLenum func, GLint ref, GLuint mask) = 0;
//  virtual void glStencilMask(GLuint mask) = 0;
//  virtual void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass) = 0;
  virtual void glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer) = 0;
//  virtual void glTexEnvi(GLenum target, GLenum pname, GLint param) = 0;
//  virtual void glTexEnvx(GLenum target, GLenum pname, GLfixed param) = 0;
//  virtual void glTexEnviv(GLenum target, GLenum pname, const GLint *params) = 0;
//  virtual void glTexEnvxv(GLenum target, GLenum pname, const GLfixed *params) = 0;
  virtual void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels) = 0;
//  virtual void glTexParameteri(GLenum target, GLenum pname, GLint param) = 0;
  virtual void glTexParameterx(GLenum target, GLenum pname, GLfixed param) = 0;
//  virtual void glTexParameteriv(GLenum target, GLenum pname, const GLint *params) = 0;
//  virtual void glTexParameterxv(GLenum target, GLenum pname, const GLfixed *params) = 0;
//  virtual void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels) = 0;
  virtual void glTranslatex(GLfixed x, GLfixed y, GLfixed z) = 0;
  virtual void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer) = 0;
  virtual void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) = 0;
};

//---------------------------------------------------------------------------
class CAVideoDevice
{
public:
  CAVideoDevice();
  virtual ~CAVideoDevice();

  virtual void listModes(const SVideoMode ** modes, int * modeCount) = 0;
  virtual void getCurrentMode(const SVideoMode ** mode) = 0;
  virtual void getDefaultMode(const SVideoMode ** mode) = 0;
  virtual void setMode(const SVideoMode * mode) = 0;

  virtual void getSurface(CSurface ** surface, int width, int height) = 0;
  virtual void get2DRenderer(I2DRenderer ** renderer) = 0;
  virtual void get3DRenderer(I3DRenderer ** renderer) = 0;

  virtual void waitVSync() = 0;
  virtual void displaySurface(CSurface * surface) = 0;

  virtual void bitBlt(CSurface * dest, int dx, int dy, int w, int h, CSurface * source, int sx, int sy) = 0;

  virtual void setVSync(bool vsync = true);
  virtual bool getVSync();

protected:
  bool vSync_;
};

//---------------------------------------------------------------------------
#define MAX_VIDEO_DEVICE_COUNT 2
class CVideoManager
{
public:
  friend class CAVideoDevice;

  CVideoManager();
  virtual ~CVideoManager();

  void listDevices(CAVideoDevice ** devices[], int * deviceCount);
  void bitBlt(CSurface * dest, int dx, int dy, int w, int h, CSurface * source, int sx, int sy);

private:
  void addDevice(CAVideoDevice * device);
  void removeDevice(CAVideoDevice * device);

  CAVideoDevice * devices_[MAX_VIDEO_DEVICE_COUNT];
  int iDeviceCount_;
};


extern CVideoManager videoManager;


#endif
