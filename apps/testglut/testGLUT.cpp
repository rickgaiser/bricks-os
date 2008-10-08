#include "GLES/gl.h"
#include "GL/glu.h"
#include "GL/glut.h"
#include "vhl/vector.h"


#define TRIANGLE_COUNT 4


const GLfloat fogColor[] = { 0.4f,  0.4f,  0.4f,  1.0f};
float xrot(20.0f);
float yrot(30.0f);
float speed(2.0f);

// Material
const GLfloat matAmbient []   = { 0.4f,  0.2f,  0.0f,  1.0f};
const GLfloat matDiffuse []   = { 0.8f,  0.4f,  0.0f,  1.0f};
const GLfloat matSpecular[]   = { 0.8f,  0.4f,  0.0f,  1.0f};
const GLfloat matShininess    = 8.0f;
// Light
const GLfloat lightAmbient [] = { 1.0f,  1.0f,  1.0f,  1.0f};
const GLfloat lightDiffuse [] = { 1.0f,  1.0f,  1.0f,  1.0f};
const GLfloat lightSpecular[] = { 1.0f,  1.0f,  1.0f,  1.0f};
const GLfloat lightPosition[] = {-5.0f, 10.0f, 10.0f,  1.0f};


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
GLfloat pyramidNormalF[TRIANGLE_COUNT*3*3];

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

  // Precalculated normals
  TVector3<GLfloat> V[3];
  TVector3<GLfloat> normal;
  for(int i(0); i < TRIANGLE_COUNT; i++)
  {
    // Load vetices V0, V1 and V2
    for(int v(0); v < 3; v++)
    {
      V[v].x = pyramidVertF[i*3*3+(v*3)+0];
      V[v].y = pyramidVertF[i*3*3+(v*3)+1];
      V[v].z = pyramidVertF[i*3*3+(v*3)+2];
    }

    // Calculate normal
    normal = (V[0] - V[1]).getCrossProduct(V[2] - V[1]);
    normal.normalize();

    // Store normal for V0, V1 and V2
    for(int v(0); v < 3; v++)
    {
      pyramidNormalF[i*3*3+(v*3)+0] = normal.x;
      pyramidNormalF[i*3*3+(v*3)+1] = normal.y;
      pyramidNormalF[i*3*3+(v*3)+2] = normal.z;
    }
  }

  // Material
  glMaterialfv(GL_FRONT, GL_AMBIENT,   matAmbient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE,   matDiffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
  glMaterialf (GL_FRONT, GL_SHININESS, matShininess);

  // Lighting
  glLightfv(GL_LIGHT0, GL_AMBIENT,  lightAmbient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  lightDiffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
  glEnable(GL_LIGHT0);
}

// -----------------------------------------------------------------------------
void
display()
{
  glVertexPointer(3, GL_FLOAT, 0, pyramidVertF);
  glColorPointer (4, GL_FLOAT, 0, pyramidColF);
  glNormalPointer(   GL_FLOAT, 0, pyramidNormalF);
  glEnableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);

  glClear(GL_COLOR_BUFFER_BIT);

  glLoadIdentity();
  glPushMatrix();

    glTranslatef(0.0f, 0.0f, -6.0f);
    glRotatef(xrot, 1.0f, 0.0f, 0.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);

    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    // GLES
    //glDrawArrays(GL_TRIANGLES, 0, TRIANGLE_COUNT*3);
    // GL
    glBegin(GL_TRIANGLES);
      for(int i(0); i < (TRIANGLE_COUNT*3); i++)
      {
        //glColor4fv (&pyramidColF   [i*4]);
        glNormal3fv(&pyramidNormalF[i*3]);
        glVertex3fv(&pyramidVertF  [i*3]);
      }
    glEnd();

    glDisable(GL_LIGHTING);
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
