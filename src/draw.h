#ifndef DRAW_H
#define DRAW_H
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include "player.h"
#include "collision.h"

extern int NUM_BLOCKS;
extern Object* blocks[];
extern int paintedLightIsOn;
extern GLfloat light_position2[];
extern GLfloat light_direction2[];


extern void drawCube(Object o);
extern void drawWithColor(Object o);
extern void drawBullets(void);
extern void map();
extern void initCubes();
extern void lightSetup();
extern void materialSetup();
extern void psychedelic();
#endif
