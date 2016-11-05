#include "player.h"
#include "miscfunc.h"

const Object playerInit={
    .posx=0, .posy=1, .posz=2,
    .vx={0,0}, .vy={0.0}, .vz={0,0},
    .length=0.5, .height=1.5, .width=0.5,
    .color={0,0,0}
};

State state={
    .jumping=0
};

Object player;

extern Object player;
extern int dt;

float rotWorld;
Val2f viewAzimuth;
Val2f viewElevation;
float eyex, eyey, eyez;
float lookAtx, lookAty, lookAtz;
float upx, upy, upz;
float playerHeadHeight=0.2;
float JUMP_V=0.1;

static float* moveRightCam(void);
static float* moveForwardCam(void);

void movePlayer(){
    float * r=moveRightCam();
    float * f=moveForwardCam();
    static float cameraVForward=0;
    static float cameraVSide=0;
    cameraVForward=approach(player.vz.goal, cameraVForward, dt/(float)5000);
    cameraVSide=approach(player.vx.goal, cameraVSide, dt/(float)5000);

    /*levo-desno u odnosu na kameru*/
    player.posx+=r[0]*cameraVSide;
    player.posz+=r[2]*cameraVSide;

    /*napred-nazad*/
    player.posz+=f[2]*cameraVForward;
    player.posx+=f[0]*cameraVForward;
}

/*pomeranje levo-desno u odnosu na kameru*/
static float* moveRightCam(void)
{
    /*todo normalize func*/
    static float v[3];
    float ax=lookAtx-eyex;
    float ay=lookAty-eyey;
    float az=lookAtz-eyez;

    v[0]=ay*upz -upy*az;
    v[1]=0;
    v[2]=ax*upy - ay*upx;

    return v;
}

/*pomeranje napred-nazad u odnosu na  kameru*/
static float* moveForwardCam(void)
{
    /*todo normalize func*/
    static float v[3];
    v[0]=lookAtx-eyex;
    v[1]=lookAty-eyey;
    v[2]=lookAtz-eyez;

    return v;
}
