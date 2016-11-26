#include "keyboard.h"

static int KEY_W=0;
static int KEY_S=0;
static int KEY_A=0;
static int KEY_D=0;
float initWindowHeight=800;
static float viewAzimuthdt=15, viewElevationdt=5;
float aspectRatio=16/9.0;

void onMouseButton(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON) {

		if (state == GLUT_DOWN) {
			firePaint();
		}
	}
}

static float prev_mouse_X=500;
static float prev_mouse_Y=500;
/*nakon poziva funkcije glutWarpPointer se postavi kursor u centar.
Zbog toga glutmainloop opet pozove onMouseLook i tako u krug.
Da bi se izignorisalo dejstvo tog poziva prekida se izvrsavanje funkcije
ako je kursor u centru*/
/*indikator da li prozor igre ne dopusta kursoru misa da ga napusti. overly attached*/
static int releaseMouse=0;
void onMouseLook(int x, int y)
{
    int width=glutGet(GLUT_WINDOW_WIDTH);
    int height=glutGet(GLUT_WINDOW_HEIGHT);
    if (x == width/2 && y == height/2)
        return;
    float deltaX;
    float deltaY;
    if (releaseMouse){
        deltaX = x - prev_mouse_X;
        deltaY = y - prev_mouse_Y;
    }else{
        deltaX = x - width/2;
        deltaY = y - height/2;
        glutWarpPointer(width/2, height/2);
    }
    prev_mouse_X=x;
    prev_mouse_Y=y;
    // printf("deltax: %f, deltay: %f\n",deltaX,deltaY);
    viewAzimuth.curr+=deltaX*viewAzimuthdt/100;
    viewElevation.curr-=deltaY*viewElevationdt/100;
    /*proveri da l su azimut i elevacija 0 do 360 i -max do max*/
    if (viewAzimuth.curr>=360 ){
        viewAzimuth.curr-=360;
        viewAzimuth.goal-=360;
    }else if (viewAzimuth.curr<0){
        viewAzimuth.curr+=360;
    }
    if (viewElevation.curr>MAX_ELEVATION)
        viewElevation.curr=MAX_ELEVATION;
    else if (viewElevation.curr<-MAX_ELEVATION)
        viewElevation.curr=-MAX_ELEVATION;
}

/*isto kao i onMouseLook, sem sto se poziva umesto nje
kada je pritisnuto dugme misa*/
void onMousePressedLook(int x, int y)
{
    onMouseLook(x,y);
}

void onKeyHold()
{
    /*ako ide brzo a nije na narandzastom */
    if (fabsf(player.vz.curr>0.1) && !state.goFast)
        return;
    float bonus=(state.goFast)? 0.1 : 0;
    if (KEY_W)
        player.vz.goal=0.05 + bonus;
    if (KEY_S)
        player.vz.goal=-0.05 - bonus;
    if (KEY_A)
        player.vx.goal=-0.05 - bonus;
    if (KEY_D)
        player.vx.goal=0.05 + bonus;
}

/*ne treba mi vise al nek stoji za ukras*/
void onSpecialInputUp(int key, int x, int y)
{
    switch(key)
    {
        default:
            break;
    }
}

void onSpecialInput(int key, int x, int y)
{
    switch(key)
    {
        case (GLUT_KEY_RIGHT):
            viewAzimuth.curr+=viewAzimuthdt;
            break;
        case (GLUT_KEY_LEFT):
            viewAzimuth.curr-=viewAzimuthdt;
            break;
        case (GLUT_KEY_UP):
            viewElevation.curr+=viewElevationdt;
            if (viewElevation.curr>MAX_ELEVATION)
                viewElevation.curr=MAX_ELEVATION;
            break;
        case (GLUT_KEY_DOWN):
            viewElevation.curr-=viewElevationdt;
            if (viewElevation.curr<-MAX_ELEVATION)
                viewElevation.curr=-MAX_ELEVATION;
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
            break;
        case(' '):
            if (!state.jumping){
                player.vy.goal=JUMP_V+state.bigJump*0.1;
                state.bigJump=0;
                state.jumping=1;
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
            resetGame();
            glutFullScreen();
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
        case ('4'):
            printf("%f, %f, %f\n",player.posx,player.posy,player.posz);
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
    glutReshapeWindow(width, height);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float) width / height, 0.1, 100.0);
}
