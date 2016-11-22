#include "draw.h"
GLfloat light_position2[] = { -30, 20, -100, -1 };
GLfloat light_direction2[] = { 0, -1, 0};

/******************/
static const float scale=2;
Object* createBlock()
{

    Object* novi= (Object*)malloc(sizeof(Object));
    if (novi==NULL)
        return NULL;
    novi->length=novi->height=novi->width=scale;
    /*dodaj random boje*/

    novi->color[0]=(float)rand()/(float)(RAND_MAX);
    novi->color[1]=(float)rand()/(float)(RAND_MAX);
    novi->color[2]=(float)rand()/(float)(RAND_MAX);
    return novi;
}

int NUM_BLOCKS=0;
Object* blocks[1000];
/*za pocetak zamilsjam da su proslednjeni celi brojevi*/
#define SWAP(x, y, T) do { T SWAP = x; x = y; y = SWAP; } while (0)
void addBlocks(float begx, float endx, float begy, float endy, float begz, float endz)
{
    /*if (begx>endx || begy>endy || begz>endz){
        printf("nisi dobro prosledio\n");
        return;
    }*/
    if (begx>endx) SWAP(begx,endx,float);
    if (begy>endy) SWAP(begy,endy,float);
    if (begz>endz) SWAP(begz,endz,float);
    int count=0;
    begx*=2, endx*=2, begy*=2, endy*=2, begz*=2, endz*=2;//scale
    float x,y,z;
    for (x=begx; x<=endx; x+=scale){
        for (y=begy; y<=endy; y+=scale){
            for (z=begz; z<=endz; z+=scale){
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
    /* u pitanju je poziciono svetlo*/
    GLfloat light_position[] = { 3, 10, 10, 1 };
    GLfloat light_direction[] = { -1, -1, 0};
    GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 1 };
    GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1 };
    GLfloat light_specular[] = { 0.9, 0.9, 0.9, 1 };
    /* Ukljucuje se osvjetljenje i podesavaju parametri svetla. */
    glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 90.0);




    GLfloat light_ambient2[] = { 0.3, 0.3, 0.3, 0.2};
    GLfloat light_diffuse2[] = { 0.4, 0.4, 0.4, 0.2 };
    GLfloat light_specular2[] = { 0.2, 0.2, 0.2, 0.1 };

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT2);
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

    /*azuriranje lookat na osnovu levo-desno rotacije*/
    lookAtx=cos(M_PI*viewAzimuth.curr/180);
    lookAtz=sin(M_PI*viewAzimuth.curr/180);
    /*azuriranje lookat na osnovu gore-dole rotacije*/
    // lookAty=sin(M_PI*viewElevation.curr/180);
    lookAty=2*sin(M_PI*viewElevation.curr/180);
    /*usmerava se pogled relativno od pozicije kamere*/
    //printf("%f\n",sqrt(lookAty));
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
    int i;
    for (i=0;i<NUM_CUBES;i++){
        drawCube(cubes[i]);
    }
    for (i=0;i<NUM_BLOCKS;i++){
        drawCube(*blocks[i]);
    }
}

void initCubes()
{
    float val=2;
    /*zid iza*/
    addBlocks(-4,4,0,2,6,6);
    /*2 rupe ispod*/
    addBlocks(-1,1,-2,-2,2,2);
    /*iza starta ploce*/
    addBlocks(-4,4,-1,-1,3,5);
    /*sa leve i desne strane starta*/
    addBlocks(-4,-2,-1,-1,0,2);
    addBlocks(4,2,-1,-1,0,2);
    //addBlocks(-4,4,1,1,-6,-14);
    /*platforma koja se boji u plavo*/
    addBlocks(-4,4,-1,-1,-1,-5);
    /*zidovi oko platforme koja se boji u narandzasto*/
    addBlocks(-5,-5,-1,3,-6,-17);
    addBlocks(5,5,-1,3,-6,-17);
    /*narandzasta platforma*/
    addBlocks(-4,4,1,1,-6,-13);
    /*help da se vratim*/
    addBlocks(-4,4,-1,0,-14,-14);
    /*landing nakon sprinta*/
    addBlocks(-2,2,1,1,-23,-18);
    /*da se vratim kad padnem sa sprinta*/
    addBlocks(-4,4,-2,-2,-15,-17);



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
