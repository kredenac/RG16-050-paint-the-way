#include "draw.h"
GLfloat light_position2[] = { 0, -1, 0, 1 };
GLfloat light_direction2[] = { 0, -1, 0};
int paintedLightIsOn;

/******************/
static const float scale=2;
static float sizex,sizey,sizez;
Object* createBlock()
{
    Object* novi= (Object*)malloc(sizeof(Object));
    if (novi==NULL)
        return NULL;
    novi->length=sizex;
    novi->height=sizey;
    novi->width=sizez;

    novi->color[0]=(float)rand()/(float)(RAND_MAX);
    novi->color[1]=(float)rand()/(float)(RAND_MAX);
    novi->color[2]=(float)rand()/(float)(RAND_MAX);
    return novi;
}

int NUM_BLOCKS=11;
Object* blocks[1000];


#define SWAP(x, y, T) do { T SWAP = x; x = y; y = SWAP; } while (0)

void addBlocks(float begx, float endx, float begy, float endy, float begz, float endz)
{

    if (begx>endx) SWAP(begx,endx,float);
    if (begy>endy) SWAP(begy,endy,float);
    if (begz>endz) SWAP(begz,endz,float);
    int count=0;
    begx*=scale, endx*=scale, begy*=scale, endy*=scale, begz*=scale, endz*=scale;
    float x,y,z;
    for (x=begx; x<=endx; x+=sizex){
        for (y=begy; y<=endy; y+=sizey){
            for (z=begz; z<=endz; z+=sizez){
                blocks[NUM_BLOCKS]=createBlock();
                blocks[NUM_BLOCKS]->posx=x;
                blocks[NUM_BLOCKS]->posy=y;
                blocks[NUM_BLOCKS]->posz=z;
                NUM_BLOCKS++;
                count++;
            }
        }
    }
    printf("novi blokovi:%d ukupno:%d\n",count,NUM_BLOCKS);
}

void lightSetup()
{
    /* u pitanju je poziciono svetlo
    GLfloat light_position[] = { 3, 10, 10, 1 };
    GLfloat light_direction[] = { -1, -1, 0};
    GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 1 };
    GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1 };
    GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 90.0);*/
    GLfloat light_ambient2[] = { 0.3, 0.3, 0.3, 0.2};
    GLfloat light_diffuse2[] = { 0.4, 0.4, 0.4, 0.2 };
    GLfloat light_specular2[] = { 0.2, 0.2, 0.2, 0.1 };

    glEnable(GL_LIGHTING);
    paintedLightIsOn ? glEnable(GL_LIGHT2) : glDisable(GL_LIGHT2);
    glLightfv(GL_LIGHT2, GL_POSITION, light_position2);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light_direction2);
    glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse2);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular2);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 69.0);

}

void materialSetup()
{
    /* Koeficijenti ambijentalne refleksije materijala. */
    GLfloat ambient_coeffs[] = { 0.2, 0.2, 0.2, 1 };
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
    float s=0.3;
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

    /*azuriranje lookat na osnovu levo-desno rotacije. A3*/
    float cosy=cos(M_PI*viewElevation.curr/180);
    lookAtx=cos(M_PI*viewAzimuth.curr/180)*cosy;
    lookAtz=sin(M_PI*viewAzimuth.curr/180)*cosy;
    /*azuriranje lookat na osnovu gore-dole rotacije*/
    lookAty=sin(M_PI*viewElevation.curr/180);
    /*usmerava se pogled relativno od pozicije kamere*/
    // printf("lookatx%f lookaty%f lookatz%f\n",lookAtx,lookAty,lookAtz);
    lookAtx=eyex+lookAtx;
    lookAtz=eyez+lookAtz;
    lookAty=eyey+lookAty;

    gluLookAt(eyex, eyey, eyez,
       lookAtx, lookAty, lookAtz,
       upx, upy, upz);
}

void map()
{
    if (state.finishedGame){
        psychedelic(60);
    }
    int i;
    for (i=0;i<NUM_BLOCKS;i++){
        drawCube(*blocks[i]);
    }
}
/*boji sve kocke random bojama. interval je broj poziva funkciji potreban da se promene boje*/
void psychedelic(int interval)
{
    static int c=0;
    c++;
    if (c!=interval)
        return;
    c=0;
    int i;
    for(i=0; i<NUM_BLOCKS; i++){
        setColor(blocks[i], (float)rand()/(float)(RAND_MAX),
            (float)rand()/(float)(RAND_MAX), (float)rand()/(float)(RAND_MAX));
    }
}
void initCubes()
{
    int i;
    for (i=11;i<NUM_BLOCKS;i++){
        free(blocks[i]);
    }
    NUM_BLOCKS=11;
    sizex=sizey=sizez=2;
    float val=2;

    /*veliki levi i desni zidovi*/
    sizex=2,sizey=4,sizez=4;
    addBlocks(-5,-5,0.5,6,5,-25);
    addBlocks(5,5,0.5,6,5,-25);
    /*zid iza*/
    sizex=4,sizey=4,sizez=4;
    addBlocks(-4,4,0.5,6,6.5,6.5);
    sizex=sizey=sizez=2;
    /*2 rupe ispod*/
    addBlocks(-1,1,-2,-2,2,2);
    /*iza starta ploce*/
    addBlocks(-4,4,-1,-1,3,5);
    /*sa leve i desne strane starta*/
    addBlocks(-4,-2,-1,-1,0,2);
    addBlocks(4,2,-1,-1,0,2);

    /*platforma koja se boji u plavo*/
    addBlocks(-4,4,-1,-1,-1,-5);
    /*zid izmedju plave i narandzaste*/
    addBlocks(-4,4,0,0,-6,-6);

    /*narandzasta platforma*/
    addBlocks(-4,4,1,1,-6,-13);
    /*help da se vratim*/
    addBlocks(-4,4,-1,-1,-14,-14);
    /*da ne bude rupa*/
    addBlocks(-4,4,0,0,-13,-13);
    addBlocks(-5,-5,-1,-1,-15,-20);
    addBlocks(5,5,-1,-1,-15,-20);

    /*landing nakon sprinta*/
    addBlocks(-2,2,1,1,-18,-18);
    addBlocks(-2,2,2,2,-21,-19);
    /*da se vratim kad padnem sa sprinta*/
    addBlocks(-4,4,-2,-2,-15,-20);
    /*zid od dole do landinga*/
    sizex=4,sizey=4,sizez=2;
    addBlocks(-4,4,-1,1,-21,-21);
    sizex=sizey=sizez=2;
    /*platforma koja se boji u plavo i narandzasto*/
    addBlocks(-4,4,2,2,-22,-25);
    /*zid iza */
    sizex=4,sizey=4,sizez=2;
    addBlocks(-4,4,0.5,6,-26,-26);
    sizex=sizey=sizez=2;
    /*landing na poslednju plat*/
    addBlocks(-1,1,4.0625,4.125,-14,-14);
    sizex=2,sizey=0.25,sizez=2;
    addBlocks(-1,1,4.625,4.625,-13,-13.5);
    addBlocks(-1,1,4.75,4.75,-12.5,-13);
    addBlocks(-1,1,4.875,4.875,-12,-12.5);
    addBlocks(-1,1,5,5,-11.5,-12);
    sizex=sizey=sizez=2;

    /*finish*/
    addBlocks(-3,3,5,5,-7,-11);
    /*border*/
    addBlocks(-1,1,6,6,-10,-10);
    addBlocks(-1,-1,6,6,-9,-9);
    addBlocks(1,1,6,6,-9,-9);

    /*sredi ovu rugobu*/
    static Object cubes[11];
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
    /*velika dole*/
    cubes[10]=(Object){
        .posx=0, .posy=-2.5*val, .posz=val,
        .length=20*val, .height=val, .width=32*val,
        .color={0.1,0,0.3}
    };

    for (i=0; i<11; i++){
        blocks[i]=&cubes[i];
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

void drawCube(Object o)
{
    drawWithColor(o);
    glPushMatrix();
    glTranslatef(o.posx, o.posy, o.posz);
    glScalef(o.length,o.height,o.width);
    glutSolidCube(1);
    glPopMatrix();
}
