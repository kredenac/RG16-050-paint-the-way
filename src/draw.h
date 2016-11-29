#ifndef DRAW_H
#define DRAW_H
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include "player.h"
#include "collision.h"

extern int NUM_BLOCKS;
extern Object* blocks[];
extern int lightOn[];
#define MAX_LIGHTS 8

extern void addBlocks(float begx, float endx, float begy, float endy, float begz, float endz);
extern void setSizes(float x, float y, float z);
extern void drawCube(Object* o);
extern void drawWithColor(Object* o);
extern void drawBullets(void);
extern void map(void);
extern void initCubes(void);
extern void lightSetup(void);
extern void materialSetup(void);
extern void psychedelic(int interval);
extern void setLightPos(int n, float x, float y, float z);
extern void initLights();
#endif
