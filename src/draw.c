#include "draw.h"
float light_position[] = { 0, -1, 0, 1 };
float light_direction[] = { 0, -1, 0};
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
    //printf("novi blokovi:%d ukupno:%d\n",count,NUM_BLOCKS);
}

int lightOn[MAX_LIGHTS];
int lights[]={GL_LIGHT0,GL_LIGHT1,GL_LIGHT2,GL_LIGHT3,
    GL_LIGHT4,GL_LIGHT5,GL_LIGHT6,GL_LIGHT7};

void lightSetup()
{
    glEnable(GL_LIGHTING);
    int i;
    for (i=0; i<MAX_LIGHTS; i++){
        if (lightOn[i]){
            glEnable(lights[i]);
        }else{
            glDisable(lights[i]);
        }
    }
}
void initLights()
{
    float light_ambient[] = { 0.3, 0.3, 0.3, 1};
    float light_diffuse[] = { 0.7, 0.7, 0.7, 1 };
    float light_specular[] = { 0.2, 0.2, 0.2, 0.1 };
    int i;
    for (i=0; i<MAX_LIGHTS; i++){
        glLightfv(lights[i], GL_SPOT_DIRECTION, light_direction);
        glLightfv(lights[i], GL_AMBIENT, light_ambient);
        glLightfv(lights[i], GL_DIFFUSE, light_diffuse);
        glLightfv(lights[i], GL_SPECULAR, light_specular);
        /*opadanje intenziteta svetlosti u zavisnosti od razdaljine*/
        glLightf(lights[i], GL_CONSTANT_ATTENUATION, 0);
        glLightf(lights[i], GL_LINEAR_ATTENUATION, 0.3);
        glLightf(lights[i],GL_QUADRATIC_ATTENUATION, 0.01);
        glLightf(lights[i], GL_SPOT_CUTOFF, 180.0);
    }
}

void setLightPos(int n, float x, float y, float z)
{
    float light_position[4];
    light_position[0] =x;
    light_position[1] =y;
    light_position[2] =z;
    light_position[3] =1;
    glLightfv(lights[n], GL_POSITION, light_position);
}

void materialSetup()
{
    /* Koeficijenti ambijentalne refleksije materijala. */
    GLfloat ambient_coeffs[] = { 0, 0, 0, 1 };
    //GLfloat ambient_coeffs[] = { 0.2, 0.2, 0.2, 1 };
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
    GLfloat diffuse_coeffs[] = {o.color[0], o.color[1], o.color[2], 1 };
    /*potamnjuje ambinet ceffs*/
    float s=0;//0.3;
    GLfloat ambient_coeffs[] = {o.color[0]*s, o.color[1]*s, o.color[2]*s, 1 };
    GLfloat emission_coeffs[] = { 0.5, 0.5, 0.5, 1 };
    GLfloat emission_coeffs2[] = { 0, 0, 0, 0 };
    if (getColor(o)==WHITE){
        glMaterialfv(GL_FRONT, GL_EMISSION, emission_coeffs);
    }else{
        glMaterialfv(GL_FRONT, GL_EMISSION, emission_coeffs2);
    }
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
    if (c<interval)
        return;
    c=0;
    int i;
    /*poput gradijenta-pocetni se boje pretezno start bojom, a poslednji end bojom*/
    float start[]={(float)rand()/(float)(RAND_MAX), (float)rand()/(float)(RAND_MAX), (float)rand()/(float)(RAND_MAX)};
    float end[]={(float)rand()/(float)(RAND_MAX), (float)rand()/(float)(RAND_MAX), (float)rand()/(float)(RAND_MAX)};
    /*ipak, nece biti cist gradijent nego odstupanja. nice je deo boje od gradijenta, a displace deo od rand odstupanja*/
    float nice=0.8;
    float displace=1-nice;
    float r,g,b;
    /*s je deo start boje, a e je deo end boje*/
    for(i=0; i<NUM_BLOCKS; i++){
        float e=(float)i/NUM_BLOCKS;
        float s=1-e;
        if (getColor(*blocks[i])!=WHITE){
            r=start[0]*s + end[0]*e;
            g=start[1]*s + end[1]*e;
            b=start[2]*s + end[2]*e;
            r=r*nice + displace*(float)rand()/(float)(RAND_MAX);
            g=g*nice + displace*(float)rand()/(float)(RAND_MAX);
            b=b*nice + displace*(float)rand()/(float)(RAND_MAX);
            setColor(blocks[i],r,g,b);
        }
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
    addBlocks(-5,-5,0.5,9,5,-25);
    addBlocks(5,5,0.5,9,5,-25);
    /*veliki napred*/
    sizex=4,sizey=4,sizez=2;
    addBlocks(-4,4,0.5,9,-26,-26);
    /*veliki iza*/
    sizex=4,sizey=4,sizez=4;
    addBlocks(-4,4,0.5,9,6.5,6.5);
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
    addBlocks(0,0,5.7,5.7,-8,-8);
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
