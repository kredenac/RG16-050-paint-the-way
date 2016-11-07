#include "draw.h"

void lightSetup()
{
    /* u pitanju je direkcionalno svetlo*/
    GLfloat light_position[] = { 3, 10, 10, 1 };
    GLfloat light_direction[] = { -1, -1, -1};
    GLfloat light_ambient[] = { 0.1, 0.1, 0.1, 1 };
    GLfloat light_diffuse[] = { 0.7, 0.7, 0.6, 1 };
    GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };
    /* Ukljucuje se osvjetljenje i podesavaju parametri svetla. */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 90.0);
}

void materialSetup()
{
    /* Koeficijenti ambijentalne refleksije materijala. */
    GLfloat ambient_coeffs[] = { 0.1, 0.1, 0.1, 1 };
    /* Koeficijenti difuzne refleksije materijala. */
    GLfloat diffuse_coeffs[] = { 0.4, 0.4, 0.4, 1 };
    /* Koeficijenti spekularne refleksije materijala. */
    GLfloat specular_coeffs[] = { 1, 1, 1, 1 };
    GLfloat shininess = 20;

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void drawWithColor(Object o)
{
    GLfloat diffuse_coeffs[] = { o.color[0],o.color[1],o.color[2], 1 };
    /*potamnjuje ambinet ceffs*/
    float s=0.1;
    GLfloat ambient_coeffs[] = { o.color[0]*s,o.color[1]*s,o.color[2]*s, 1 };
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
}

/*postavlja kameru na poziciju glave igraca i usmerava pogled*/
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


void drawAxis(void)
{
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
    cubes[6]=(Object){
        .posx=0, .posy=-val, .posz=val,
        .length=val, .height=val, .width=val,
        .color={0.2,0.2,0.3}
    };
    cubes[7]=(Object){
        .posx=0, .posy=-1.2*val, .posz=2*val,
        .length=val, .height=val, .width=val,
        .color={0.3,0.4,0.5}
    };
    cubes[8]=(Object){
        .posx=-val, .posy=-val, .posz=val,
        .length=val, .height=val, .width=val,
        .color={0,0.2,0}
    };
    cubes[9]=(Object){
        .posx=val, .posy=-val, .posz=val,
        .length=val, .height=val, .width=val,
        .color={0,0.2,0}
    };
    cubes[10]=(Object){
        .posx=0, .posy=-val, .posz=3*val,
        .length=val, .height=val, .width=val,
        .color={0,0.2,0}
    };
    cubes[11]=(Object){
        .posx=0, .posy=-6*val, .posz=val,
        .length=10*val, .height=10*val, .width=10*val,
        .color={0.1,0,0.3}
    };
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

void drawCube(Object o)
{
    drawWithColor(o);
    glPushMatrix();
    glTranslatef(o.posx, o.posy, o.posz);
    glutSolidCube(o.length);
    glPopMatrix();
}
