#include "GLES/gl.h"
#include "GL/glu.h"
#include "GL/glut.h"


#define TRIANGLE_COUNT 4


const GLfloat fogColor[] = { 0.4f,  0.4f,  0.4f,  1.0f};
float xrot(20.0f);
float yrot(30.0f);
float speed(2.0f);


// -----------------------------------------------------------------------------
const GLfloat pyramidVertF[TRIANGLE_COUNT*3*3] =
{
  // Pyramid (triangles)
   0.0f,  1.0f,  0.0f,
  -1.0f, -1.0f,  1.0f,
   1.0f, -1.0f,  1.0f,

   0.0f,  1.0f,  0.0f,
   1.0f, -1.0f,  1.0f,
   1.0f, -1.0f, -1.0f,

   0.0f,  1.0f,  0.0f,
   1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,

   0.0f,  1.0f,  0.0f,
  -1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f,
};

// -----------------------------------------------------------------------------
const GLfloat pyramidColF[TRIANGLE_COUNT*3*4] =
{
  // Pyramid
  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,

  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,

  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,

  1.0f, 0.0f, 0.0f, 1.0f,
  0.0f, 0.0f, 1.0f, 1.0f,
  0.0f, 1.0f, 0.0f, 1.0f,
};

// -----------------------------------------------------------------------------
void
init()
{
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  glClearColor(fogColor[0], fogColor[1], fogColor[2], fogColor[3]);
//  glClearDepth(1.0f);
//  glEnable(GL_DEPTH_TEST);
//  glDepthFunc(GL_LEQUAL);
//  glEnable(GL_COLOR_MATERIAL);
//  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

// -----------------------------------------------------------------------------
void
display()
{
  glVertexPointer(3, GL_FLOAT, 0, pyramidVertF);
  glColorPointer (4, GL_FLOAT, 0, pyramidColF);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);

  glClear(GL_COLOR_BUFFER_BIT);

  glLoadIdentity();
  glPushMatrix();

    glTranslatef(0.0f, 0.0f, -6.0f);
    glRotatef(xrot, 1.0f, 0.0f, 0.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);

    glShadeModel(GL_SMOOTH);
    // GLES
    //glDrawArrays(GL_TRIANGLES, 0, TRIANGLE_COUNT*3);
    // GL
    glBegin(GL_TRIANGLES);
      for(int i(0); i < (TRIANGLE_COUNT*3); i++)
      {
        glColor4fv (&pyramidColF [i*4]);
        glVertex3fv(&pyramidVertF[i*3]);
      }
    glEnd();

    glShadeModel(GL_FLAT);
    glBegin(GL_TRIANGLES);
      glColor4f ( 0.6f,  0.6f,  0.6f,  1.0f);

      glVertex3f(-1.0f, -1.0f,  1.0f);
      glVertex3f(-1.0f, -1.0f, -1.0f);
      glVertex3f( 1.0f, -1.0f,  1.0f);

      glVertex3f( 1.0f, -1.0f,  1.0f);
      glVertex3f(-1.0f, -1.0f, -1.0f);
      glVertex3f( 1.0f, -1.0f, -1.0f);
    glEnd();

  glPopMatrix();

  glFlush();
  glutSwapBuffers();
}

// -----------------------------------------------------------------------------
void
reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (float)w / (float)h, 0.1f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

// -----------------------------------------------------------------------------
void
keyboard(unsigned char key, int x, int y)
{
  switch(key)
  {
    default:
      break;
  };
}

// -----------------------------------------------------------------------------
void
arrow_keys(int a_keys, int x, int y)
{
  switch(a_keys)
  {
    case GLUT_KEY_UP:
      xrot -= 1.0f * speed;
      if(xrot < 0.0f)
        xrot += 360.0f;
      break;
    case GLUT_KEY_DOWN:
      xrot += 1.0f * speed;
      if(xrot >= 360.0f)
        xrot -= 360.0f;
      break;
    case GLUT_KEY_LEFT:
      yrot -= 1.0f * speed;
      if(yrot < 0.0f)
        yrot += 360.0f;
      break;
    case GLUT_KEY_RIGHT:
      yrot += 1.0f * speed;
      if(yrot >= 360.0f)
        yrot -= 360.0f;
      break;
    default:
      break;
  };
}

// -----------------------------------------------------------------------------
extern "C" int
appMain(int argc, char * argv[])
{
  glutInit(&argc, argv);
  init();
//  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(arrow_keys);
//  glutIdleFunc(display);
  glutMainLoop();

  return 0;
}
