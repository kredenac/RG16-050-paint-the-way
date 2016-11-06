#include "player.h"
#include "miscfunc.h"

const Object playerInit={
    .posx=0, .posy=1, .posz=2,
    .vx={0,0}, .vy={0.0}, .vz={0,0},
    .length=0.5, .height=1.5, .width=0.5,
    .color={0,0,0}
};
const Object bulletInit={
    .posx=0, .posy=0, .posz=0,
    .vx={0,0}, .vy={0.0}, .vz={0,0},
    .length=0.1, .height=0.1, .width=0.1,
    .color={1,1,1}
};
State state={
    .jumping=0,
    .fireColor=1
};

Object player;
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


Object bullets[MAX_BULLETS];
int bullets_active[MAX_BULLETS];

#define BULLET_SPEED 10
void firePaint()
{
    int i=0;
    float r,g,b;
    for (i=0;i<MAX_BULLETS;i++){
        /*u prvi neaktivan slot ubaci*/
        if (!bullets_active[i]){
            bullets_active[i]=1;
            bullets[i]=bulletInit;
            bullets[i].posx=player.posx;
            bullets[i].posy=player.posy;
            bullets[i].posz=player.posz;
            /*postavljanje boje metka na osnovu izabrane*/
            switch(state.fireColor){
                case(1):
                    r=1,g=1,b=1;
                    break;
                case(2):
                    r=0,g=0,b=1;
                    break;
                case(3):
                    r=1,g=0.3,b=0.1;
                    break;
                default:
                    r=1,g=1,b=1;
                    break;
            }
            setColor(&bullets[i], r, g, b);
            /*postavljanje vektora brzine metka na tacku gde igrac gleda*/
            float * v=moveForwardCam();
            bullets[i].vx.curr=v[0]/BULLET_SPEED;
            bullets[i].vy.curr=v[1]/BULLET_SPEED;
            bullets[i].vz.curr=v[2]/BULLET_SPEED;
            /*izlazi iz petlje jer je postavljen metak*/
            break;
        }
    }
}

void resetBullets()
{
    int i;
    for (i=0;i<MAX_BULLETS;i++){
        bullets_active[i]=0;
    }
}
