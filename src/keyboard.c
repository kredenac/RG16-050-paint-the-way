#include "keyboard.h"

static float viewAzimuthdt=30, viewElevationdt=20;
float aspectRatio=16/9.0;
/*mora jos da se podesava da lepo radi*/
void mouseLook(int x, int y)
{
    printf("x: %d, y: %d\n",x,y);
    static float prev_mouse_X=0;
    static float prev_mouse_Y=0;
    float deltaX = x - prev_mouse_X;
    float deltaY = y - prev_mouse_Y ;
    if ( deltaX < -6.0f ) { deltaX = -6.0f; }
    if ( deltaX >  6.0f ) { deltaX =  6.0f; }
    if ( deltaY < -6.0f ) { deltaY = -6.0f; }
    if ( deltaY >  6.0f ) { deltaY =  6.0f; }
    prev_mouse_X=x;
    prev_mouse_Y=y;
    printf("deltax: %f, deltay: %f\n",deltaX,deltaY);
    viewAzimuth.goal=viewAzimuth.curr+deltaX*viewAzimuthdt/100;
    viewElevation.goal=viewElevation.curr-deltaY*viewElevationdt/100;
    printf("acurr%f, agoal%f\n",viewAzimuth.curr,viewAzimuth.goal);

}
void onSpecialInputUp(int key, int x, int y)
{
    switch(key)
    {
        case (GLUT_KEY_RIGHT):
            viewAzimuth.goal=viewAzimuth.curr;
            break;
        case (GLUT_KEY_LEFT):
            viewAzimuth.goal=viewAzimuth.curr;
            break;
        case (GLUT_KEY_UP):
            viewElevation.goal=viewElevation.curr;
            break;
        case (GLUT_KEY_DOWN):
            viewElevation.goal=viewElevation.curr;
            break;
    }
}


void onSpecialInput(int key, int x, int y)
{

    switch(key)
    {
        case (GLUT_KEY_RIGHT):
            viewAzimuth.goal+=viewAzimuthdt;
            break;
        case (GLUT_KEY_LEFT):
            viewAzimuth.goal-=viewAzimuthdt;
            break;
        case (GLUT_KEY_UP):
            viewElevation.goal+=viewElevationdt;
            if (viewElevation.goal>MAX_ELEVATION)
                viewElevation.goal=MAX_ELEVATION;
            break;
        case (GLUT_KEY_DOWN):
            viewElevation.goal-=viewElevationdt;
            if (viewElevation.goal<-MAX_ELEVATION)
                viewElevation.goal=-MAX_ELEVATION;
            break;
    }

}

void onKeyboardUp(unsigned char key, int x, int y)
{
    switch(key){
        case('a'):
            player.vx.goal=0;
            break;
        case('d'):
            player.vx.goal=0;
            break;
        case('w'):
            player.vz.goal=0;
            break;
        case('s'):
            player.vz.goal=0;
            break;
        case(' '):
            player.vy.goal=0;
            break;
    }
}

void onKeyboard(unsigned char key, int x, int y)
{

    switch(key){
        case('f'):
            firePaint();
            break;
        case(' '):
            if (!state.jumping){
                player.vy.goal=JUMP_V;
                state.jumping=1;
            }
            break;
        case('S'):
            if (!animationOngoing){
                resetGame();
            }else{
                freezeGame();
            }
            break;
        case('s'):
            player.vz.goal=-0.05;
            break;
        case('w'):
            player.vz.goal=0.05;
            break;
        case('a'):
            player.vx.goal=-0.05;
            break;
        case('d'):
            player.vx.goal=0.05;
            break;
        case('r'):
            resetGame();
            break;
        case ('m'):
            rotWorld+=10;
            break;
        case ('n'):
            rotWorld-=10;
            break;
        case ('1'):
            state.fireColor=1;
            break;
        case ('2'):
            state.fireColor=2;
            break;
        case ('3'):
            state.fireColor=3;
            break;
        case(27):
            exit(0);
            break;
    }
    //printf("feet pos: x=%f, y=%f, z=%f\n",player.posx,player.posy-player.height/2,player.posz);
}


void onReshape(int width, int height)
{
    width=aspectRatio*height;
    glutReshapeWindow(width, height);
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float) width / height, 0.1, 100.0);
}
