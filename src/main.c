#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "player.h"
#include "keyboard.h"
#include "miscfunc.h"
#include "collision.h"
#include "draw.h"

static void onDisplay(void);
int dt;
void onTimerUpdate(int id);
static void updateDeltaTime(void);
static void fps(int print);

/*TODO boje mogu da budu malo vise fun*/
int main(int argc, char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowSize(aspectRatio*initWindowHeight,initWindowHeight);
    glutInitWindowPosition(0,0);
    glutCreateWindow("Paint the Way");
    glutDisplayFunc(onDisplay);
    glutReshapeFunc(onReshape);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutSpecialFunc(onSpecialInput);
    glutSpecialUpFunc(onSpecialInputUp);
    glutPassiveMotionFunc(onMouseLook);
    glutMotionFunc(onMousePressedLook);
    glutMouseFunc(onMouseButton);

    glEnable(GL_CULL_FACE);
    /*prilikom skaliranja se poremeti osvetljenje pa me gl_normalize spasi*/
    glEnable(GL_NORMALIZE);
    glutSetCursor(GLUT_CURSOR_NONE);
    glutIgnoreKeyRepeat(GL_TRUE);

    glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
    glEnable(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glClearColor(0, 0, 0, 0);
    resetGame();
    dt=glutGet(GLUT_ELAPSED_TIME);
    srand(time(NULL));
    glutTimerFunc(UPDATE_TIMER_INTVAL, onTimerUpdate,TIMER_UPDATE_ID);
    glutMainLoop();
    return 0;
}

void onDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    fps(0);
    positionCam();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    lightSetup();
    materialSetup();

    map();
    drawBullets();

    glutSwapBuffers();
}

void onTimerUpdate(int id)
{
    if (TIMER_UPDATE_ID!=id){
        return;
    }
    updateDeltaTime();
    movePlayer();
    playerCollision();
    bulletCollision();
    moveBullets();

    glutPostRedisplay();
    glutTimerFunc(UPDATE_TIMER_INTVAL, onTimerUpdate, TIMER_UPDATE_ID);
}

#define DT_MAX 60
static int newTime;
static int oldTime=0;
static int timeSum=0;
/*racunanje dt-vremena izmedju 2 poziva onTimerUpdate funkcije*/
void updateDeltaTime(void)
{
    newTime=glutGet(GLUT_ELAPSED_TIME);
    dt=newTime-oldTime;
    oldTime=newTime;
    timeSum+=dt;
    if (dt>DT_MAX)
        dt=DT_MAX;
    //printf("dt=%d\n",dt);
}

/*racuna frames per second*/
void fps(int print)
{
    static int frame=0;
    frame++;
    if (print && timeSum>=1000){
        printf("fps:%f\n",frame*1000/(float)timeSum);
        timeSum=0;
        frame=0;
    }
}
