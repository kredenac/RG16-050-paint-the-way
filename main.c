#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "player.h"
#include "keyboard.h"
#include "miscfunc.h"

static void onDisplay(void);
int dt;
void onTimerUpdate(int id);
static void updatedt(void);
static void positionCam(void);
static void drawAxis(void);
static void updateCamAngle(void);

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
    /*pls work*/
    glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
    /*pls work*/
    glClearColor(0, 0, 0, 0);

    dt=glutGet(GLUT_ELAPSED_TIME);
    srand(time(NULL));
    resetGame();
    glutMainLoop();
    return 0;
}
/*void room(float size)
{
    glPushMatrix();
    glScalef(5,5,5);
    glBegin(GL_QUADS);
        glColor3f(0,0,0);
        glVertex3f(1,-1,-1);
        glColor3f(0,0,0);
        glVertex3f(-1,-1,-1);
        glColor3f(r,g,b);
        glVertex3f(-1,-1,1);
        glColor3f(r,g,b);
        glVertex3f(1,-1,1);
    glEnd();
    glPopMatrix();
}*/
void onDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*ajde jbt*/
    positionCam();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    glRotatef(rotWorld,0,1,0);

    static float r=0.5,g=0.5,b=0.5;
    /*6 zidova sobe-strane kocke*/
    //room(5);
    glPushMatrix();

    glScalef(7,7,7);
    glPushMatrix();
    glTranslatef(0,-1,0);
    glColor3f(r,g,b);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,1,0);
    glColor3f(0,0,0.3);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1,0,0);
    glColor3f(0,0.4,0);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-1,0,0);
    glColor3f(0,0.4,0);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0,0,-1);
    glColor3f(0.3,0,0);
    glutSolidCube(1);
    glPopMatrix();
    /*glBegin(GL_QUADS);
        glColor3f(0,0,0);
        glVertex3f(1,-1,-1);
        glColor3f(0,0,0);
        glVertex3f(-1,-1,-1);
        glColor3f(r,g,b);
        glVertex3f(-1,-1,1);
        glColor3f(r,g,b);
        glVertex3f(1,-1,1);
    glEnd();*/

    glPopMatrix();
    drawAxis();

    /*TEST iscrtavanje metka i pomeranje*/
    float x=bullets[0].posx;
    float y=bullets[0].posy;
    float z=bullets[0].posz;
    float vx=bullets[0].vx.curr;
    float vy=bullets[0].vy.curr;
    float vz=bullets[0].vz.curr;

    float br=bullets[0].color[0];
    float bg=bullets[0].color[1];
    float bb=bullets[0].color[2];
    if (bullets_active[0]){
        glTranslatef(x,y,z);
        glColor3f(br,bg,bb);
        glutSolidSphere(0.2, 10, 10);
        glTranslatef(-x,-y,-    z);
    }
    bullets[0].posx+=vx;
    bullets[0].posy+=vy;
    bullets[0].posz+=vz;

    if (bullets[0].posy<=-5){
        bullets_active[0]=0;
        r=br;
        g=bg;
        b=bb;
    }
    /*TEST*/

    glTranslatef(-1.6, 0, -3);
        glColor3f(1, 0, 1);
        glRotatef(180,0,1,0);
        glutWireTeapot(0.5);
    glTranslatef(1.6, 0, 1);

    glutSwapBuffers();
}


const float FLOOR=-1;

void onTimerUpdate(int id)
{
    if (TIMER_UPDATE_ID!=id){
        return;
    }
    updatedt();
    /*pomeraj*/
    player.vy.curr=approach(player.vy.goal, player.vy.curr, dt/(float)500);
    player.posy+=player.vy.curr;
    /*kolizija*/

    /*kolizija sa podom*/
    player.posy-=0.02;//gravity

    if (player.posy - player.height/2<=FLOOR) {
        state.jumping=0;
        player.posy=FLOOR+player.height/2;
        player.vy.curr=0;
    }

    movePlayer();
    /*rotacija kamere*/
    updateCamAngle();

    glutPostRedisplay();
    if (animationOngoing){
        glutTimerFunc(UPDATE_TIMER_INTVAL, onTimerUpdate, TIMER_UPDATE_ID);
    }
}

void updateCamAngle(void)
{
    viewAzimuth.curr=approach(viewAzimuth.goal, viewAzimuth.curr, dt/(float)20);
    //printf("curr:%f goal:%f\n",viewAzimuth.curr,viewAzimuth.goal);
    if (viewAzimuth.curr>=360 && viewAzimuth.goal>=360){
        viewAzimuth.curr-=360;
        viewAzimuth.goal-=360;
    }else if (viewAzimuth.curr<0 && viewAzimuth.goal<0){
        viewAzimuth.curr+=360;
        viewAzimuth.goal+=360;
    }
    viewElevation.curr=approach(viewElevation.goal, viewElevation.curr, dt/(float)20);
    if (viewElevation.curr>MAX_ELEVATION){
        viewElevation.curr=MAX_ELEVATION;
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

void positionCam(void)
{
    eyex=player.posx;
    eyey=player.posy+playerHeadHeight;
    eyez=player.posz;

    /*azuriranje lookat na osnovu levo-desno rotacije*/
    lookAtx=cos(M_PI*viewAzimuth.curr/180);
    lookAtz=sin(M_PI*viewAzimuth.curr/180);
    /*azuriranje lookat na osnovu gore-dole rotacije*/
    lookAty=sin(M_PI*viewElevation.curr/180);
    /*usmerava se pogled relativno od pozicije kamere*/
    lookAtx=eyex+lookAtx;
    lookAtz=eyez+lookAtz;
    lookAty=eyey+lookAty;
    gluLookAt(eyex, eyey, eyez,
       lookAtx, lookAty, lookAtz,
       upx, upy, upz);
}

void drawAxis(void)
{
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
}
