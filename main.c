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
int hasCollision(Object a, Object b);
int rangeIntersect(float mina, float maxa, float minb, float maxb);
void drawCube(Object o);
void drawWithColor(Object o);
/*bice izbrisane funkcije:*/
void room(float size);
#define NUM_CUBES 6
Object cubes[6];
void initCubes()
{
    float val=2;
    cubes[0]=(Object){
        .posx=-val, .posy=0, .posz=0,
        .length=val, .height=val, .width=val,
        .color={0.2,0,0}
    };
    cubes[1]=(Object){
        .posx=val, .posy=0, .posz=0,
        .length=val, .height=val, .width=val,
        .color={0.3,0,0}
    };
    cubes[2]=(Object){
        .posx=0, .posy=-val, .posz=0,
        .length=val, .height=val, .width=val,
        .color={0,0.2,0}
    };
    cubes[3]=(Object){
        .posx=0, .posy=val, .posz=0,
        .length=val, .height=val, .width=val,
        .color={0,0.3,0}
    };
    cubes[4]=(Object){
        .posx=0, .posy=0, .posz=-val,
        .length=val, .height=val, .width=val,
        .color={0,0,0.2}
    };
    cubes[5]=(Object){
        .posx=0, .posy=0, .posz=0,
        .length=0.5, .height=0.5, .width=0.5,
        .color={0,0,0}
    };
}
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

    glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

    glClearColor(0, 0, 0, 0);
    initCubes();
    dt=glutGet(GLUT_ELAPSED_TIME);
    srand(time(NULL));
    resetGame();
    glutMainLoop();
    return 0;
}

/*todo skloni da ne budu na ovako ruznom mestu ove vars*/
float tr=0.5,tg=0.5,tb=0.5;
void moveBullets(void)
{
    int i;
    for (i=0;i<MAX_BULLETS;i++)
        if (bullets_active[i]){
            bullets[i].posx+=bullets[i].vx.curr;
            bullets[i].posy+=bullets[i].vy.curr;
            bullets[i].posz+=bullets[i].vz.curr;
            /*kvazi kolizija*/
            if (bullets[i].posy<=-5){//-5 jer mi je tu pod.. fix it
                bullets_active[i]=0;
                tr=bullets[i].color[0];
                tg=bullets[i].color[1];
                tb=bullets[i].color[2];
            }
    }
}

void drawBullets(void)
{
    float x, y, z;
    int i;
    for (i=0;i<MAX_BULLETS;i++){
        if (bullets_active[i]){
            x=bullets[i].posx,y=bullets[i].posy,z=bullets[i].posz;
            glPushMatrix();
            glTranslatef(x,y,z);
            drawWithColor(bullets[i]);
            glutSolidSphere(bullets[i].length, 20, 10);
            glPopMatrix();
        }
    }
}

int rangeIntersect(float mina, float maxa, float minb, float maxb)
{
    /*mozda da proverim da l je proslednjeni min zaista manji od max?*/
    return maxa>=minb && mina<=maxb;
}
/*proverava da li dva objekta imaju presek po svakoj osi,
* ako imaju to je kolizija*/
int hasCollision(Object a, Object b)
{
    float mina,maxa,minb,maxb;
    mina=a.posx-a.length/2;
    maxa=a.posx+a.length/2;
    minb=b.posx-b.length/2;
    maxb=b.posx+b.length/2;
    int interx=rangeIntersect(mina, maxa, minb, maxb);

    mina=a.posy-a.height/2;
    maxa=a.posy+a.height/2;
    minb=b.posy-b.height/2;
    maxb=b.posy+b.height/2;
    int intery=rangeIntersect(mina, maxa, minb, maxb);

    mina=a.posz-a.width/2;
    maxa=a.posz+a.width/2;
    minb=b.posz-b.width/2;
    maxb=b.posz+b.width/2;
    int interz=rangeIntersect(mina, maxa, minb, maxb);


    return interx && intery && interz;
}
void drawCube(Object o)
{
    drawWithColor(o);
    glPushMatrix();
    glTranslatef(o.posx, o.posy, o.posz);
    glutSolidCube(o.length);
    glPopMatrix();
}

void drawWithColor(Object o)
{
    GLfloat diffuse_coeffs[] = { o.color[0],o.color[1],o.color[2], 1 };
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
}
void onDisplay(void)
{
    /* Pozicija svetla (u pitanju je direkcionalno svetlo). */
    GLfloat light_position[] = { 1, 1, 1, 0 };
    /* Ambijentalna boja svetla. */
    GLfloat light_ambient[] = { 0.1, 0.1, 0.5, 1 };
    /* Difuzna boja svetla. */
    GLfloat light_diffuse[] = { 0.7, 0.7, 0.6, 1 };
    /* Spekularna boja svetla. */
    GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };

    /* Koeficijenti ambijentalne refleksije materijala. */
    GLfloat ambient_coeffs[] = { 0.1, 0.1, 0.1, 1 };
    /* Koeficijenti difuzne refleksije materijala. */
    GLfloat diffuse_coeffs[] = { 0, 0, 0.7, 1 };
    /* Koeficijenti spekularne refleksije materijala. */
    GLfloat specular_coeffs[] = { 1, 1, 1, 1 };

    GLfloat shininess = 20;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    positionCam();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(rotWorld,0,1,0);

    /* Ukljucuje se osvjetljenje i podesavaju parametri svetla. */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
    /*****/
    /*6 zidova sobe-strane kocke*/
    room(1);
    drawAxis();
    drawBullets();

    glTranslatef(-1.6, 0, -3);
        glColor3f(1, 0, 1);
        glRotatef(180,0,1,0);
        glutWireTeapot(0.5);
    glTranslatef(1.6, 0, 1);

    glutSwapBuffers();
}

void room(float size)
{
    glPushMatrix();
    glScalef(size,size,size);
    int i;
    for (i=0;i<NUM_CUBES;i++){
        drawCube(cubes[i]);
    }
    glPopMatrix();
}


static float FLOOR=-1;
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
    int i,j;
    /*prodji kroz sve slotove, ako je aktivan metak proveri koliziju sa kockama*/
    for (i=0; i<MAX_BULLETS; i++){
        if (bullets_active[i]){
            for (j=0; j<NUM_CUBES; j++){
                if (hasCollision(cubes[j],bullets[i])){
                    bullets_active[i]=0;
                    setColor(&cubes[j],bullets[i].color[0],bullets[i].color[1],bullets[i].color[2]);
                    break;
                }
            }
        }
    }
    /*kolizija sa podom*/
    player.posy-=0.02;//gravity

    if (player.posy - player.height/2<=FLOOR) {
        state.jumping=0;
        player.posy=FLOOR+player.height/2;
        player.vy.curr=0;
    }
    moveBullets();
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
