#ifndef MISCFUNC_H
#define MISCFUNC_H
#include <math.h>
#include <stdio.h>
#include <GL/glut.h>
#include "player.h"
#define TIMER_UPDATE_ID 1
#define UPDATE_TIMER_INTVAL 17

extern float approach(float goal, float curr, float dt);
extern void setColor(Object* op, float r, float g, float b);
extern void resetGame(void);
extern void freezeGame(void);
extern int animationOngoing;

#endif
