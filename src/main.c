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
static void updatedt(void);

int main(int argc, char ** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    float windowHeight=800;
    glutInitWindowSize(aspectRatio*windowHeight,windowHeight);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Paint the Way");
    glutDisplayFunc(onDisplay);
    glutReshapeFunc(onReshape);
    glutKeyboardFunc(onKeyboard);
    glutKeyboardUpFunc(onKeyboardUp);
    glutSpecialFunc(onSpecialInput);
    glutSpecialUpFunc(onSpecialInputUp);
    glutPassiveMotionFunc(mouseLook);


    glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

    glClearColor(0, 0, 0, 0);
    initCubes();
    dt=glutGet(GLUT_ELAPSED_TIME);
    srand(time(NULL));
    resetGame();
    glutTimerFunc(UPDATE_TIMER_INTVAL, onTimerUpdate,TIMER_UPDATE_ID);
    glutMainLoop();
    return 0;
}

void onDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    positionCam();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(rotWorld,0,1,0);

    lightSetup();
    materialSetup();

    room(1);
    drawAxis();
    drawBullets();

    glutSwapBuffers();
}

void onTimerUpdate(int id)
{
    if (TIMER_UPDATE_ID!=id){
        return;
    }
    updatedt();
    movePlayer();
    playerCollision();
    bulletCollision();
    moveBullets();

    updateCamAngle();

    glutPostRedisplay();
    if (animationOngoing){
        glutTimerFunc(UPDATE_TIMER_INTVAL, onTimerUpdate, TIMER_UPDATE_ID);
    }
}

#define DT_MAX 60
/*racunanje dt-vremena izmedju 2 frejma. verovatno mi ni nece trebati*/
void updatedt(void)
{
    static int newTime;
    newTime=glutGet(GLUT_ELAPSED_TIME);
    static int oldTime=0;
    dt=newTime-oldTime;
    oldTime=newTime;
    if (dt>DT_MAX)
        dt=DT_MAX;
}
