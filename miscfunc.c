#include <math.h>
#include <stdio.h>
#include <GL/glut.h>
#include "miscfunc.h"
#include "player.h"
int animationOngoing=1;

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

void setColor(Object* op, float r, float g, float b)
{
    op->color[0]=r;
    op->color[1]=g;
    op->color[2]=b;
}

extern void onTimerUpdate(int id);
void resetGame(void)
{
    player=playerInit;
    animationOngoing=1;
    rotWorld=0;
    viewAzimuth.curr=270, viewAzimuth.goal=270;
    viewElevation.curr=-10, viewElevation.goal=-10;
    eyex=0, eyey=1, eyez=2;
    lookAtx=0, lookAty=0, lookAtz=0;
    upx=0, upy=1, upz=0;
    glutTimerFunc(UPDATE_TIMER_INTVAL, onTimerUpdate,TIMER_UPDATE_ID);
    player=playerInit;
}

void freezeGame(void)
{
    animationOngoing=0;
    printf("Press shift+S to try again\n");
}
