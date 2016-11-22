#include "keyboard.h"

static int KEY_W=0;
static int KEY_S=0;
static int KEY_A=0;
static int KEY_D=0;
float windowHeight=800,windowWidth;
static float viewAzimuthdt=30, viewElevationdt=10;
float aspectRatio=16/9.0;

void onMouseButton(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {

		if (state == GLUT_DOWN) {
			firePaint();
		}
		else  {// state = GLUT_UP


		}
	}
}
static float prev_mouse_X=500;
static float prev_mouse_Y=500;
/*nakon poziva funkcije glutWarpPointer se postavi kursor u centar.
Zbog toga glutmainloop opet pozove onMouseLook i tako u krug.
Da bi se izignorisalo dejstvo tog poziva promenljiva warped se ukljuci nakon
poziva glutWarpPointer i postavi na nulu pri sledecem ulasku u onMouseLook*/
static int warped=0;
/*indikator da li prozor igre ne dopusta kursoru misa da ga napusti. overly attached*/
static int releaseMouse=0;
void onMouseLook(int x, int y)
{
    if (warped){
        warped=0;
        return;
    }
    float deltaX;
    float deltaY;
    if (releaseMouse){
        deltaX = x - prev_mouse_X;
        deltaY = y - prev_mouse_Y;
    }else{
        deltaX = x - windowWidth/2;
        deltaY = y - windowHeight/2;
        glutWarpPointer(windowWidth/2, windowHeight/2);
        warped=1;
    }
    prev_mouse_X=x;
    prev_mouse_Y=y;
    // printf("deltax: %f, deltay: %f\n",deltaX,deltaY);
    viewAzimuth.curr+=deltaX*viewAzimuthdt/100;
    viewElevation.curr-=deltaY*viewElevationdt/100;

    if (viewElevation.curr>MAX_ELEVATION)
        viewElevation.curr=MAX_ELEVATION;
    if (viewElevation.curr<-MAX_ELEVATION)
            viewElevation.curr=-MAX_ELEVATION;
    viewAzimuth.goal=viewAzimuth.curr;
    viewElevation.goal=viewElevation.curr;
    // printf("acurr%f, agoal%f\n",viewAzimuth.curr,viewAzimuth.goal);
}
/*isto kao i onMouseLook, sem sto se poziva umesto nje
kada je pritisnuto dugme*/
void onMousePressedLook(int x, int y)
{
    if (warped){
        warped=0;
        return;
    }
    float deltaX;
    float deltaY;
    if (releaseMouse){
        deltaX = x - prev_mouse_X;
        deltaY = y - prev_mouse_Y;
    }else{
        deltaX = x - windowWidth/2;
        deltaY = y - windowHeight/2;
        glutWarpPointer(windowWidth/2, windowHeight/2);
        warped=1;
    }
    prev_mouse_X=x;
    prev_mouse_Y=y;
    viewAzimuth.curr+=deltaX*viewAzimuthdt/100;
    viewElevation.curr-=deltaY*viewElevationdt/100;

    if (viewElevation.curr>MAX_ELEVATION)
        viewElevation.curr=MAX_ELEVATION;
    if (viewElevation.curr<-MAX_ELEVATION)
            viewElevation.curr=-MAX_ELEVATION;
    viewAzimuth.goal=viewAzimuth.curr;
    viewElevation.goal=viewElevation.curr;
}

void onKeyHold()
{
    //int bonus=(state.goFast)? 0.1 : 0;
    /*ako ide brzo a nije na narandzastom */
    if (fabsf(player.vz.curr>0.1) && !state.goFast)
        return;
    if (KEY_W)
        player.vz.goal=0.05+state.goFast*0.1;
    if (KEY_S)
        player.vz.goal=-0.05-state.goFast*0.1;
    if (KEY_A)
        player.vx.goal=-0.05-state.goFast*0.1;
    if (KEY_D)
        player.vx.goal=0.05+state.goFast*0.1;
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
            KEY_A=0;
            player.vx.goal=0;
            break;
        case('d'):
            KEY_D=0;
            player.vx.goal=0;
            break;
        case('w'):
            KEY_W=0;
            player.vz.goal=0;
            break;
        case('s'):
            KEY_S=0;
            player.vz.goal=0;
            break;

    }
}

void onKeyboard(unsigned char key, int x, int y)
{

    switch(key){
        case('f'):
            firePaint();
            glutWarpPointer(windowWidth/2, windowHeight/2);
            break;
        case(' '):
            if (!state.jumping){
                player.vy.goal=JUMP_V+state.bigJump*0.1;
                state.bigJump=0;
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
            KEY_S=1;
            player.vz.goal=-0.05-state.goFast*0.15;
            break;
        case('w'):
            KEY_W=1;
            player.vz.goal=0.05+state.goFast*0.15;
            break;
        case('a'):
            KEY_A=1;
            player.vx.goal=-0.05-state.goFast*0.15;
            break;
        case('d'):
            KEY_D=1;
            player.vx.goal=0.05+state.goFast*0.15;
            break;
        case('r'):
            glutFullScreen();
            resetGame();
            break;
        case ('1'):
            state.fireColor=WHITE;
            break;
        case ('2'):
            state.fireColor=BLUE;
            break;
        case ('3'):
            state.fireColor=ORANGE;
            break;
        case('p'):
            if (!releaseMouse){
                glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
                releaseMouse=1;
            }else{
                glutSetCursor(GLUT_CURSOR_NONE);
                releaseMouse=0;
            }
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
    windowHeight=height;
    windowWidth=width;
    glutReshapeWindow(width, height);
    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float) width / height, 0.1, 100.0);
}
