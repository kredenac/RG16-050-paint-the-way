#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "player.h"
#include "keyboard.h"
#include "miscfunc.h"

int dt;
static void onDisplay(void);
void drawSquare(Object o);
void onTimerUpdate(int id);
void TimerSpawnWall(int id);


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

    glClearColor(0.1, 0.1, 0.1, 0);

    dt=glutGet(GLUT_ELAPSED_TIME);
    player=playerInit;
    srand(time(NULL));
    if (animationOngoing){
        glutTimerFunc(UPDATE_TIMER_INTVAL, onTimerUpdate, TIMER_UPDATE_ID);
    }
    glutMainLoop();
    return 0;
}

void onDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    eyex=player.posx;
    eyey=player.posy+playerHeadHeight;
    eyez=player.posz;

    /*azuriranje lookat na osnovu levo-desno rotacije*/
    lookAtx=cos(M_PI*viewAzimuth.curr/180);
    lookAtz=sin(M_PI*viewAzimuth.curr/180);
    /*azuriranje lookat na osnovu gore-dole rotacije*/
    lookAty=sin(M_PI*viewElevation.curr/180);

    lookAtx=eyex+lookAtx;
    lookAtz=eyez+lookAtz;
    lookAty=eyey+lookAty;
    gluLookAt(eyex, eyey, eyez,
       lookAtx, lookAty, lookAtz,
       upx, upy, upz);
    glRotatef(rotWorld,0,1,0);

    glPushMatrix();
    glScalef(3,3,3);
    glBegin(GL_QUADS);
        glColor3f(0,0,1);
        glVertex3f(1,-1,-1);
        glColor3f(0,0,0);
        glVertex3f(-1,-1,-1);
        glColor3f(1,0,0);
        glVertex3f(-1,-1,1);
        glColor3f(1,1,1);
        glVertex3f(1,-1,1);
    glEnd();
    glPopMatrix();
    /*x,y,z, ose*/
    glBegin(GL_LINES);
        glColor3f(1,1,1);
        glVertex3f(-1,0,0);
        glColor3f(1,0,0);
        glVertex3f(1,0,0);
    glEnd();

    glBegin(GL_LINES);
        glColor3f(1,1,1);
        glVertex3f(0,-1,0);
        glColor3f(0,1,0);
        glVertex3f(0,1,0);
    glEnd();

    glBegin(GL_LINES);
        glColor3f(1,1,1);
        glVertex3f(0,0,-1);
        glColor3f(0,0,1);
        glVertex3f(0,0,1);
    glEnd();
    /**/


    glTranslatef(-1.6, 0, -3);

        glColor3f(1, 0, 1);
        glRotatef(180,0,1,0);
        glutWireTeapot(0.5);
    glTranslatef(1.6, 0, 1);

    glutSwapBuffers();
}

#define DT_MAX 60
const float FLOOR=-1;

void onTimerUpdate(int id)
{
    if (TIMER_UPDATE_ID!=id){
        return;
    }

    /*racunanje dt-vremena izmedju 2 frejma. verovatno mi ni nece trebati*/
    static int newTime;
    newTime=glutGet(GLUT_ELAPSED_TIME);
    static int oldTime=0;
    dt=newTime-oldTime;
    oldTime=newTime;
    if (dt>DT_MAX)
        dt=DT_MAX;

    /*pomeraj*/
    player.vy.curr=approach(player.vy.goal, player.vy.curr, dt/(float)500);
    player.posy+=player.vy.curr;
    /*kolizija*/
    /*kolizija sa podom*/
    player.posy-=0.02;//gravity

    if (player.posy - player.height/2<FLOOR) {
        player.posy=FLOOR+player.height/2;
        player.vy.curr=0;
    }
    /*brzine*/
    movePlayer();
    /*rotacija kamere*/
    viewAzimuth.curr=approach(viewAzimuth.goal, viewAzimuth.curr, dt/(float)20);

    viewElevation.curr=approach(viewElevation.goal, viewElevation.curr, dt/(float)20);
    if (viewElevation.curr>MAX_ELEVATION){
        viewElevation.curr=MAX_ELEVATION;
    }
    glutPostRedisplay();
    if (animationOngoing){
        glutTimerFunc(UPDATE_TIMER_INTVAL, onTimerUpdate, TIMER_UPDATE_ID);
    }
}
