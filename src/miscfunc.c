
#include "miscfunc.h"
int showFps=0;

static int isequal(float a, float b);
/*funkcija linearne interpolacije. sluzi da se postepeno menja vrednost*/
float approach(float goal, float curr, float dt)
{
    float diff=goal-curr;
    if (diff>dt){
        return curr+dt;
    }else if(diff<-dt){
        return curr-dt;
    }
    /*else je u dometu dt*/
    return goal;
}

void normalize3f(float* x, float* y, float* z)
{
    float n=sqrt(*x**x+*y**y+*z**z);
    *x=*x/n;
    *y=*y/n;
    *z=*z/n;
}

void setColor(Object* op, float r, float g, float b)
{

    op->color[0]=r;
    op->color[1]=g;
    op->color[2]=b;
}

void set3fWithColor(Color c,float* r1, float* g1, float* b1)
{
    float r,g,b;
    switch(c){
        case(WHITE):
            r=1,g=1,b=1;
            break;
        case(BLUE):
            r=0,g=0,b=1;
            break;
        case(ORANGE):
            r=1,g=0.3,b=0.1;
            break;
        default:
            r=1,g=1,b=1;
            break;
    }
    *r1=r, *g1=g, *b1=b;
}

Color getColor(Object o)
{
    float col[3];
    col[0]=o.color[0];
    col[1]=o.color[1];
    col[2]=o.color[2];

    if (isequal(col[0],1) && isequal(col[1],1) && isequal(col[2],1) )
        return WHITE;
    else if (isequal(col[0],0) && isequal(col[1],0) && isequal(col[2],1) ){
        return BLUE;
    }else if (isequal(col[0],1) && isequal(col[1],0.3) && isequal(col[2],0.1)){
        return ORANGE;
    }

    return OTHER;
}
/*zbog reprezentacije float-a cesto ne budu jednaki kada poredim*/
const float eps=0.01;
int isequal(float a, float b)
{
    if (fabsf(a-b)<eps)
        return 1;
    return 0;
}

void resetGame(void)
{
    resetBullets();
    initCubes();
    initLights();
    state=stateInit;
    player=playerInit;
    viewAzimuth.curr=270;
    viewElevation.curr=0;
    eyex=0, eyey=1, eyez=2;
    lookAtx=0, lookAty=0, lookAtz=0;
    upx=0, upy=1, upz=0;
    int i;
    for (i=0;i<MAX_LIGHTS; i++)
        lightOn[i]=0;
}
