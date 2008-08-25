#ifndef GLUT_H
#define GLUT_H


#define  GLUT_KEY_LEFT                      0x0064
#define  GLUT_KEY_UP                        0x0065
#define  GLUT_KEY_RIGHT                     0x0066
#define  GLUT_KEY_DOWN                      0x0067


void glutInit        (int * pargc, char ** argv);
void glutDisplayFunc (void (* callback)(void));
void glutReshapeFunc (void (* callback)(int, int));
void glutKeyboardFunc(void (* callback)(unsigned char, int, int));
void glutSpecialFunc (void (* callback)(int, int, int));
void glutMainLoop    (void);
void glutSwapBuffers (void);


#endif
