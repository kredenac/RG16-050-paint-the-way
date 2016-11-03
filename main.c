#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>


typedef struct{
    float posx;
    float posy;
    float posz;
    float vx;
    float vy;
    float vz;
    float height;
    float width;
    float color[3];
}Object;


#define TIMER_COLOR_ID 2
#define TIMER_UPDATE_ID 0
#define UPDATE_TIMER_INTVAL 17


static int dt;
static float JUMP_V=0.1;
static float playerGoalvx;
static float playerGoalvy;
static float playerGoalvz;
static float aspectRatio=1/*16/9.0*/;
static int animationOngoing=1;
static Object player;
static const Object playerInit={0, 1, 0,
    0, 0, 0,
    1, 0.1,
    {0,0,0} };

static void onDisplay(void);
static void onReshape(int width, int height);
static void onKeyboard(unsigned char key, int x, int y);
static void onKeyboardUp(unsigned char key, int x, int y);
static void onSpecialInput(int key, int x, int y);
static void onSpecialInputUp(int key, int x, int y);

float* moveRightCam(void);
float* moveForwardCam(void);
float approach(float goal, float curr, float dt);
void drawSquare(Object o);
void onTimerUpdate(int id);
void TimerSpawnWall(int id);
void setColor(Object* op, float r, float g, float b);
void freezeGame(void);
void resetGame(void);


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
        glutTimerFunc(UPDATE_TIMER_INTVAL, onTimerUpdate,TIMER_UPDATE_ID);
    }
    glutMainLoop();
    return 0;
}

void onReshape(int width, int height)
{

    width=aspectRatio*height;
    glutReshapeWindow(width, height);
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (float) width / height, 1, 1500);
}

static float eyex=0, eyey=0.5, eyez=2;
static float rotWorld=0;
static float viewAzimuth=270, viewElevation=-10;
static float lookAtx=0, lookAty=0, lookAtz=0;
static float upx=0, upy=1, upz=0;


void onDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /*azuriranje lookat na osnovu levo-desno rotacije*/
    lookAtx=cos(M_PI*viewAzimuth/180);
    lookAtz=sin(M_PI*viewAzimuth/180);
    /*azuriranje lookat na osnovu gore-dole rotacije*/
    lookAty=sin(M_PI*viewElevation/180);

    lookAtx=eyex+lookAtx;
    lookAtz=eyez+lookAtz;
    lookAty=eyey+lookAty;
    gluLookAt(eyex, eyey,eyez ,//0,0,2
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

static float viewAzimuthGoal=270, viewElevationGoal=-10;
static float viewAzimuthdt=30, viewElevationdt=20;

static void onSpecialInputUp(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_RIGHT:
            viewAzimuthGoal=viewAzimuth;
            break;
        case GLUT_KEY_LEFT:
            viewAzimuthGoal=viewAzimuth;
            break;
        case GLUT_KEY_UP:
            viewElevationGoal=viewElevation;
            break;
        case GLUT_KEY_DOWN:
            viewElevationGoal=viewElevation;
            break;
    }
}
#define MAX_ELEVATION 89
void onSpecialInput(int key, int x, int y)
{

    switch(key)
    {
        case GLUT_KEY_RIGHT:
            viewAzimuthGoal+=viewAzimuthdt;
            break;
        case GLUT_KEY_LEFT:
            viewAzimuthGoal-=viewAzimuthdt;
            break;
        case GLUT_KEY_UP:
            viewElevationGoal+=viewElevationdt;
            if (viewElevationGoal>MAX_ELEVATION)
                viewElevationGoal=MAX_ELEVATION;
            break;
        case GLUT_KEY_DOWN:
            viewElevationGoal-=viewElevationdt;
            if (viewElevationGoal<-MAX_ELEVATION)
                viewElevationGoal=-MAX_ELEVATION;
            break;
    }

}

void onKeyboardUp(unsigned char key, int x, int y)
{
    switch(key){
        case('a'):
            playerGoalvx=0;
            break;
        case('d'):
            playerGoalvx=0;
            break;
        case('w'):
            playerGoalvz=0;
            break;
        case('s'):
            playerGoalvz=0;
            break;
        case(' '):
            playerGoalvy=0;
            break;
    }
}



void onKeyboard(unsigned char key, int x, int y)
{

    switch(key){
        case(27):
            exit(0);
            break;
        case(' '):
            playerGoalvy=JUMP_V;
            break;
        case('S'):
            if (!animationOngoing){
                resetGame();
            }else{
                freezeGame();
            }
            break;
        case('s'):
            playerGoalvz=-0.05;
            break;
        case('w'):
            playerGoalvz=0.05;
            break;
        case('a'):
            playerGoalvx=-0.05;
            break;
        case('d'):
            playerGoalvx=0.05;
            break;
        case('r'):
            player=playerInit;
            break;
        case 'm':
            rotWorld+=10;
            break;
        case 'n':
            rotWorld-=10;
            break;
    }
}



#define DT_MAX 60
#define PLAYER_HEIGHT 1

static float cameraVForward=0;
static float cameraVSide=0;
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
    player.vy=approach(playerGoalvy, player.vy, dt/(float)500);
    player.posy+=player.vy;
    /*kolizija*/
    /*kolizija sa podom*/
    player.posy-=0.02;//gravity
    eyey=player.posy;
    if (eyey - PLAYER_HEIGHT <-1) {
        player.posy=0;
        player.vy=0;
    }
    /*brzine*/

    /*vektori r i f kretanja u odnosu na kameru*/
    float * r=moveRightCam();
    float * f=moveForwardCam();
    cameraVForward=approach(playerGoalvz, cameraVForward, dt/(float)5000);
    cameraVSide=approach(playerGoalvx, cameraVSide, dt/(float)5000);

    /*levo-desno u odnosu na kameru*/
    eyex+=r[0]*cameraVSide;
    eyez+=r[2]*cameraVSide;

    /*napred-nazad*/
    eyez+=f[2]*cameraVForward;
    eyex+=f[0]*cameraVForward;
    /*rotacija kamere*/
    viewAzimuth=approach(viewAzimuthGoal, viewAzimuth, dt/(float)20);

    viewElevation=approach(viewElevationGoal, viewElevation, dt/(float)20);
    if (viewElevation>MAX_ELEVATION){
        viewElevation=MAX_ELEVATION;
    }
    glutPostRedisplay();
    if (animationOngoing){
        glutTimerFunc(UPDATE_TIMER_INTVAL, onTimerUpdate, TIMER_UPDATE_ID);
    }
}

/*pomeranje levo-desno u odnosu na kameru*/
float* moveRightCam(void)
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
float* moveForwardCam(void)
{
    /*todo normalize func*/
    static float v[3];
    v[0]=lookAtx-eyex;
    v[1]=lookAty-eyey;
    v[2]=lookAtz-eyez;

    return v;
}

/*
int collision(Object player, Wall o)
{
    int left,right,up,down;
    right=player.posx+player.width/2  > o.bot.posx - o.bot.width/2;
    left= player.posx-player.width/2  < o.bot.posx + o.bot.width/2;
    up=   player.posy+player.height/2 > o.top.posy - o.top.height/2;
    down= player.posy-player.height/2 < o.bot.posy + o.bot.height/2;
    if (right && left && (up || down)){
        return 1;
    }
    return 0;
}*/

/*funkcija linearne interpolacije. sluzi da se postepeno povecava brzina*/
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

void resetGame(void)
{
    playerGoalvx=0;
    playerGoalvy=0;
    playerGoalvz=0;
    animationOngoing=1;
    glutTimerFunc(UPDATE_TIMER_INTVAL, onTimerUpdate,TIMER_UPDATE_ID);
    player=playerInit;
}

void freezeGame(void)
{
    animationOngoing=0;
    printf("Press shift+S to try again\n");
}
