#include "glMatrix.h"
#include "string.h"
#include "math.h"


#ifdef CONFIG_FPU
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CAGLESMatrixF::CAGLESMatrixF()
 : matrixMode_(GL_MODELVIEW)
 , iCurrentModelView_(1)
 , iCurrentProjection_(1)
 , iCurrentTexture_(1)
 , pCurrentMatrix_(&matrixModelView)
{
  // All stacks contain 1 identity matrix
  stackModelView[0].loadIdentity();
  stackProjection[0].loadIdentity();
  stackTexture[0].loadIdentity();
}

//---------------------------------------------------------------------------
CAGLESMatrixF::~CAGLESMatrixF()
{
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  GLfloat m[16];

  GLfloat idw = 1.0f / (right - left);
  GLfloat idh = 1.0f / (top - bottom);
  GLfloat idz = 1.0f / (zFar - zNear);

  m[0*4+0] = (2.0f * zNear) * idw;
  m[0*4+1] = 0.0f;
  m[0*4+2] = (right + left) * idw;
  m[0*4+3] = 0.0f;

  m[1*4+0] = 0.0f;
  m[1*4+1] = (2.0f * zNear) * idh;
  m[1*4+2] = (top + bottom) * idh;
  m[1*4+3] = 0.0f;

  m[2*4+0] = 0.0f;
  m[2*4+1] = 0.0f;
  m[2*4+2] = -((zFar + zNear) * idz);
  m[2*4+3] = -((2.0f * zFar * zNear) * idz);

  m[3*4+0] = 0.0f;
  m[3*4+1] = 0.0f;
  m[3*4+2] = -1.0f;
  m[3*4+3] = 0.0f;

  *pCurrentMatrix_ *= m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glLoadMatrixf(const GLfloat *m)
{
  *pCurrentMatrix_ = m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glMultMatrixf(const GLfloat *m)
{
  *pCurrentMatrix_ *= m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
  GLfloat m[16];

  GLfloat idw = 1.0f / (right - left);
  GLfloat idh = 1.0f / (top - bottom);
  GLfloat idz = 1.0f / (zFar - zNear);

  m[0*4+0] = 2.0f * idw;
  m[0*4+1] = 0.0f;
  m[0*4+2] = 0.0f;
  m[0*4+3] = -((right + left) * idw);

  m[1*4+0] = 0.0f;
  m[1*4+1] = 2.0f * idh;
  m[1*4+2] = 0.0f;
  m[1*4+3] = -((top + bottom) * idh);

  m[2*4+0] = 0.0f;
  m[2*4+1] = 0.0f;
  m[2*4+2] = -2.0f * idz;
  m[2*4+3] = -((zFar + zNear) * idz);

  m[3*4+0] = 0.0f;
  m[3*4+1] = 0.0f;
  m[3*4+2] = 0.0f;
  m[3*4+3] = 1.0f;

  *pCurrentMatrix_ *= m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
  GLfloat ax = angle * x;
  GLfloat ay = angle * y;
  GLfloat az = angle * z;

  pCurrentMatrix_->rotate(ax, ay, az);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glScalef(GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->scale(x, y, z);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
  pCurrentMatrix_->translate(x, y, z);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glPopMatrix(void)
{
  switch(matrixMode_)
  {
    case GL_MODELVIEW:
      if(iCurrentModelView_ > 0)
      {
        iCurrentModelView_--;
        matrixModelView = stackModelView[iCurrentModelView_];
      }
      break;
    case GL_PROJECTION:
      if(iCurrentProjection_ > 0)
      {
        iCurrentProjection_--;
        matrixProjection = stackProjection[iCurrentProjection_];
      }
      break;
    case GL_TEXTURE:
      if(iCurrentTexture_ > 0)
      {
        iCurrentTexture_--;
        matrixTexture = stackTexture[iCurrentTexture_];
      }
      break;
  };
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glPushMatrix(void)
{
  switch(matrixMode_)
  {
    case GL_MODELVIEW:
      if(iCurrentModelView_ < 16)
      {
        stackModelView[iCurrentModelView_] = matrixModelView;
        iCurrentModelView_++;
      }
      break;
    case GL_PROJECTION:
      if(iCurrentProjection_ < 2)
      {
        stackProjection[iCurrentProjection_] = matrixProjection;
        iCurrentProjection_++;
      }
      break;
    case GL_TEXTURE:
      if(iCurrentTexture_ < 2)
      {
        stackTexture[iCurrentTexture_] = matrixTexture;
        iCurrentTexture_++;
      }
      break;
  };
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glLoadIdentity(void)
{
  pCurrentMatrix_->loadIdentity();
}

//---------------------------------------------------------------------------
void
CAGLESMatrixF::glMatrixMode(GLenum mode)
{
  matrixMode_ = mode;

  switch(mode)
  {
    case GL_MODELVIEW:  pCurrentMatrix_ = &matrixModelView;  break;
    case GL_PROJECTION: pCurrentMatrix_ = &matrixProjection; break;
    case GL_TEXTURE:    pCurrentMatrix_ = &matrixTexture;    break;
  };
}
#else // CONFIG_FPU
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CAGLESMatrixFx::CAGLESMatrixFx()
 : matrixMode_(GL_MODELVIEW)
 , iCurrentModelView_(1)
 , iCurrentProjection_(1)
 , iCurrentTexture_(1)
 , pCurrentMatrix_(&matrixModelView)
{
  // All stacks contain 1 identity matrix
  stackModelView[0].loadIdentity();
  stackProjection[0].loadIdentity();
  stackTexture[0].loadIdentity();
}

//---------------------------------------------------------------------------
CAGLESMatrixFx::~CAGLESMatrixFx()
{
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  CFixed m[16];

  GLfixed idw = gl_fpinverse(right - left);
  GLfixed idh = gl_fpinverse(top - bottom);
  GLfixed idz = gl_fpinverse(zFar - zNear);

  m[0*4+0].value = gl_fpmul((zNear << 1), idw);
  m[0*4+1].value = gl_fpfromi(0);
  m[0*4+2].value = gl_fpmul((right + left), idw);
  m[0*4+3].value = gl_fpfromi(0);

  m[1*4+0].value = gl_fpfromi(0);
  m[1*4+1].value = gl_fpmul((zNear << 1), idh);
  m[1*4+2].value = gl_fpmul((top + bottom), idh);
  m[1*4+3].value = gl_fpfromi(0);

  m[2*4+0].value = gl_fpfromi(0);
  m[2*4+1].value = gl_fpfromi(0);
  m[2*4+2].value = -gl_fpmul((zFar + zNear), idz);
  m[2*4+3].value = -gl_fpmul((gl_fpmul(zFar, zNear) << 1), idz);

  m[3*4+0].value = gl_fpfromi(0);
  m[3*4+1].value = gl_fpfromi(0);
  m[3*4+2].value = gl_fpfromi(-1);
  m[3*4+3].value = gl_fpfromi(0);

  *pCurrentMatrix_ *= m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glLoadMatrixx(const GLfixed * m)
{
  *pCurrentMatrix_ = (CFixed *)m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glMultMatrixx(const GLfixed * m)
{
  *pCurrentMatrix_ *= (CFixed *)m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
  CFixed m[16];

  GLfixed idw = gl_fpinverse(right - left);
  GLfixed idh = gl_fpinverse(top - bottom);
  GLfixed idz = gl_fpinverse(zFar - zNear);

  m[0*4+0].value = gl_fpmul(gl_fpfromi(2), idw);
  m[0*4+1].value = gl_fpfromi(0);
  m[0*4+2].value = gl_fpfromi(0);
  m[0*4+3].value = -gl_fpmul((right + left), idw);

  m[1*4+0].value = gl_fpfromi(0);
  m[1*4+1].value = gl_fpmul(gl_fpfromi(2), idh);
  m[1*4+2].value = gl_fpfromi(0);
  m[1*4+3].value = -gl_fpmul((top + bottom), idh);

  m[2*4+0].value = gl_fpfromi(0);
  m[2*4+1].value = gl_fpfromi(0);
  m[2*4+2].value = gl_fpmul(gl_fpfromi(-2), idz);
  m[2*4+3].value = -gl_fpmul((zFar + zNear), idz);

  m[3*4+0].value = gl_fpfromi(0);
  m[3*4+1].value = gl_fpfromi(0);
  m[3*4+2].value = gl_fpfromi(0);
  m[3*4+3].value = gl_fpfromi(1);

  *pCurrentMatrix_ *= m;
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
  CFixed fx;
  CFixed fy;
  CFixed fz;

  fx.value = gl_fpmul(angle, x);
  fy.value = gl_fpmul(angle, y);
  fz.value = gl_fpmul(angle, z);

  pCurrentMatrix_->rotate(fx, fy, fz);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glScalex(GLfixed x, GLfixed y, GLfixed z)
{
  CFixed fx;
  CFixed fy;
  CFixed fz;

  fx.value = x;
  fy.value = y;
  fz.value = z;

  pCurrentMatrix_->scale(fx, fy, fz);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
  CFixed fx;
  CFixed fy;
  CFixed fz;

  fx.value = x;
  fy.value = y;
  fz.value = z;

  pCurrentMatrix_->translate(fx, fy, fz);
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glPopMatrix(void)
{
  switch(matrixMode_)
  {
    case GL_MODELVIEW:
      if(iCurrentModelView_ > 0)
      {
        iCurrentModelView_--;
        matrixModelView = stackModelView[iCurrentModelView_];
      }
      break;
    case GL_PROJECTION:
      if(iCurrentProjection_ > 0)
      {
        iCurrentProjection_--;
        matrixProjection = stackProjection[iCurrentProjection_];
      }
      break;
    case GL_TEXTURE:
      if(iCurrentTexture_ > 0)
      {
        iCurrentTexture_--;
        matrixTexture = stackTexture[iCurrentTexture_];
      }
      break;
  };
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glPushMatrix(void)
{
  switch(matrixMode_)
  {
    case GL_MODELVIEW:
      if(iCurrentModelView_ < 16)
      {
        stackModelView[iCurrentModelView_] = matrixModelView;
        iCurrentModelView_++;
      }
      break;
    case GL_PROJECTION:
      if(iCurrentProjection_ < 2)
      {
        stackProjection[iCurrentProjection_] = matrixProjection;
        iCurrentProjection_++;
      }
      break;
    case GL_TEXTURE:
      if(iCurrentTexture_ < 2)
      {
        stackTexture[iCurrentTexture_] = matrixTexture;
        iCurrentTexture_++;
      }
      break;
  };
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glLoadIdentity(void)
{
  pCurrentMatrix_->loadIdentity();
}

//---------------------------------------------------------------------------
void
CAGLESMatrixFx::glMatrixMode(GLenum mode)
{
  matrixMode_ = mode;

  switch(mode)
  {
    case GL_MODELVIEW:  pCurrentMatrix_ = &matrixModelView;  break;
    case GL_PROJECTION: pCurrentMatrix_ = &matrixProjection; break;
    case GL_TEXTURE:    pCurrentMatrix_ = &matrixTexture;    break;
  };
}
#endif // CONFIG_FPU
