#ifndef COLLISION_H
#define COLLISION_H
#include <stdio.h>
#include <math.h>
#include "player.h"
#include "miscfunc.h"

typedef enum {
    TOP,
    BOT,
    FRONT,/*z+*/
    BACK,/*z-*/
    LEFT,/*x-*/
    RIGHT/*x+*/
}Side;
extern int hasCollision(Object a, Object b);
extern void playerCollision(void);
extern void bulletCollision(void);
#define NUM_CUBES 22
extern Object cubes[];
#endif
