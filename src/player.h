#ifndef PLAYER_H
#define PLAYER_H
#include <stdio.h>
#include <math.h>
/*miscfunc.h includuje player.h i tako u krug pa moram ovde da stavim boje*/
typedef enum {
    WHITE,
    BLUE,
    ORANGE,
    OTHER
}Color;
extern void onKeyHold();

#define MAX_ELEVATION 89
#define MAX_BULLETS 77
/*sluzi za cuvanje trenutnog i ciljnog stanja, npr brzine*/
typedef struct{
    float curr;
    float goal;
}Val2f;
/*struktura za sve kockaste objekte
* posx,y,z predstavljaju centar tela*/
typedef struct{
    float posx;
    float posy;
    float posz;
    Val2f vx;
    Val2f vy;
    Val2f vz;
    float length;
    float height;
    float width;
    float color[3];
}Object;

typedef struct{
    int jumping;
    Color fireColor;
    int bigJump;
    int goFast;
}State;
/*--------------------------*/

//extern float testx, testz;

/**---------------********/
extern State state;
extern const State stateInit;
/*relativna pozicija visine glave u odnosu na centar tela
* sluzi za pozicioniranje kamere*/
extern float playerHeadHeight;
extern const Object playerInit;
extern void movePlayer();
extern void firePaint();
extern void resetBullets();
extern void moveBullets(void);
extern void updateCamAngle(void);
extern void positionCam(void);
extern Object player;

extern float GRAVITY;
extern float rotWorld;
extern Val2f viewAzimuth;
extern Val2f viewElevation;
extern float JUMP_V;
extern Object player;
extern int animationOngoing;

extern float rotWorld;
extern Val2f viewAzimuth;
extern Val2f viewElevation;
extern float eyex, eyey, eyez;
extern float lookAtx, lookAty, lookAtz;
extern float upx, upy, upz;

extern Object bullets[];
extern int bullets_active[];
/*miscfunc.h includuje player.h i obrnuto pa se upletu ...*/
#include "miscfunc.h"
#endif
