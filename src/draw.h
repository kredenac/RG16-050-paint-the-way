#ifndef DRAW_H
#define DRAW_H
#include <GL/glut.h>
#include <math.h>
#include "player.h"
#include "collision.h"


extern GLfloat light_position2[];
extern GLfloat light_direction2[];

extern void drawAxis(void);
extern void drawCube(Object o);
extern void drawWithColor(Object o);
extern void drawBullets(void);
extern void room(float size);
extern void initCubes();
extern void lightSetup();
extern void materialSetup();
#endif
