#include "draw.h"

static const float scale = 2;
static float sizex, sizey, sizez;
void setSizes(float x, float y, float z)
{
    sizex = x;
    sizey = y;
    sizez = z;
}

Object* createBlock()
{
    Object* novi = (Object* ) malloc(sizeof(Object));
    if (novi == NULL)
        return NULL;
    novi->length = sizex;
    novi->height = sizey;
    novi->width = sizez;

    novi->color[0] = (float) rand() / (float)(RAND_MAX);
    novi->color[1] = (float) rand() / (float)(RAND_MAX);
    novi->color[2] = (float) rand() / (float)(RAND_MAX);
    return novi;
}

int NUM_BLOCKS = 0;
Object* blocks[1000];

#define SWAP(x, y, T) do {T SWAP = x; x = y; y = SWAP;} while (0)

void addBlocks(float begx, float endx, float begy, float endy, float begz, float endz)
{
    if (begx > endx) SWAP(begx, endx, float);
    if (begy > endy) SWAP(begy, endy, float);
    if (begz > endz) SWAP(begz, endz, float);
    int count = 0;
    begx *= scale, endx *= scale, begy *= scale;
    endy *= scale, begz *= scale, endz *= scale;
    float x, y, z;
    for (x = begx; x <= endx; x += sizex) {
        for (y = begy; y <= endy; y += sizey) {
            for (z = begz; z <= endz; z += sizez) {
                blocks[NUM_BLOCKS] = createBlock();
                blocks[NUM_BLOCKS]->posx = x;
                blocks[NUM_BLOCKS]->posy = y;
                blocks[NUM_BLOCKS]->posz = z;
                NUM_BLOCKS++;
                count++;
            }
        }
    }
    //printf("novi blokovi:%d ukupno:%d\n",count,NUM_BLOCKS);
}

int lightOn[MAX_LIGHTS];
int lights[] = {
    GL_LIGHT0,
    GL_LIGHT1,
    GL_LIGHT2,
    GL_LIGHT3,
    GL_LIGHT4,
    GL_LIGHT5,
    GL_LIGHT6,
    GL_LIGHT7
};
float lightPos[MAX_LIGHTS][4];

void lightSetup()
{
    glEnable(GL_LIGHTING);
    int i;
    for (i = 0; i < MAX_LIGHTS; i++) {
        if (lightOn[i]) {
            glEnable(lights[i]);
            glLightfv(lights[i], GL_POSITION, lightPos[i]);
        } else {
            glDisable(lights[i]);
        }
    }
}
void initLights()
{
    float lightAmbient[] = {0.3, 0.3, 0.3, 1    };
    float lightDiffuse[] = {0.7, 0.7, 0.7, 1};
    float lightSpecular[] = {0.2, 0.2, 0.2, 0.0}; //0.1
    float lightDirection[] = {0, 0, 0};
    int i;
    for (i = 0; i < MAX_LIGHTS; i++) {
        glLightfv(lights[i], GL_SPOT_DIRECTION, lightDirection);
        glLightfv(lights[i], GL_AMBIENT, lightAmbient);
        glLightfv(lights[i], GL_DIFFUSE, lightDiffuse);
        glLightfv(lights[i], GL_SPECULAR, lightSpecular);
        /*opadanje intenziteta svetlosti u zavisnosti od razdaljine*/
        glLightf(lights[i], GL_CONSTANT_ATTENUATION, 0);
        glLightf(lights[i], GL_LINEAR_ATTENUATION, 0.3);
        glLightf(lights[i], GL_QUADRATIC_ATTENUATION, 0.01);
        glLightf(lights[i], GL_SPOT_CUTOFF, 180.0);
    }
}

void setLightPos(int n, float x, float y, float z)
{
    lightPos[n][0] = x;
    lightPos[n][1] = y;
    lightPos[n][2] = z;
    lightPos[n][3] = 1;
}

void materialSetup()
{
    /* Koeficijenti ambijentalne refleksije materijala. */
    GLfloat ambient_coeffs[] = {0, 0, 0, 1};
    //GLfloat ambient_coeffs[] = { 0.2, 0.2, 0.2, 1 };
    /* Koeficijenti difuzne refleksije materijala. */
    GLfloat diffuse_coeffs[] = {0.4, 0.4, 0.4, 1};
    /* Koeficijenti spekularne refleksije materijala. */
    GLfloat specular_coeffs[] = {0, 0, 0, 1};
    GLfloat shininess = 0; //20

    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient_coeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse_coeffs);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular_coeffs);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);
}

void drawWithColor(Object* o)
{
    GLfloat diffuseCoeffs[] = {o->color[0], o->color[1], o->color[2],1};
    /*potamnjuje ambinet coeffs*/
    float s = 0; //0.3;
    GLfloat ambientCoeffs[] = {o->color[0] * s, o->color[1] * s, o->color[2] * s,1};
    GLfloat emissionCoeffs[] = {0.5,0.5,0.5,1};
    GLfloat emissionCoeffs2[] = {0,0,0,0};
    if (getColor(o) == WHITE) {
        glMaterialfv(GL_FRONT, GL_EMISSION, emissionCoeffs);
    } else {
        glMaterialfv(GL_FRONT, GL_EMISSION, emissionCoeffs2);
    }
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambientCoeffs);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuseCoeffs);
}

/*postavlja kameru na poziciju glave igraca i usmerava pogled*/
void positionCam(void)
{
    eyex = player.posx;
    eyey = player.posy + playerHeadHeight;
    eyez = player.posz;

    /*azuriranje lookat na osnovu levo-desno rotacije. A3*/
    float cosy = cos(M_PI * viewElevation.curr / 180);
    lookAtx = cos(M_PI * viewAzimuth.curr / 180) * cosy;
    lookAtz = sin(M_PI * viewAzimuth.curr / 180) * cosy;
    /*azuriranje lookat na osnovu gore-dole rotacije*/
    lookAty = sin(M_PI * viewElevation.curr / 180);
    /*usmerava se pogled relativno od pozicije kamere*/
    // printf("lookatx%f lookaty%f lookatz%f\n",lookAtx,lookAty,lookAtz);
    lookAtx = eyex + lookAtx;
    lookAtz = eyez + lookAtz;
    lookAty = eyey + lookAty;

    gluLookAt(eyex, eyey, eyez,
        lookAtx, lookAty, lookAtz,
        upx, upy, upz);
}

void map()
{
    int i;
    for (i = 0; i < NUM_BLOCKS; i++) {
        drawCube(blocks[i]);
    }
}

/*boji sve kocke random bojama. interval je broj poziva funkciji potreban da se promene boje*/
void psychedelic(int interval)
{
    static int c = 0;
    c++;
    if (c < interval)
        return;
    c = 0;
    int i;
    /*poput gradijenta-pocetni se boje pretezno start bojom, a poslednji end bojom*/
    float start[] = {
        rand() / (float)(RAND_MAX),
        rand() / (float)(RAND_MAX),
        rand() / (float)(RAND_MAX)
    };
    float end[] = {
        rand() / (float)(RAND_MAX),
        rand() / (float)(RAND_MAX),
        rand() / (float)(RAND_MAX)
    };
    /*ipak, nece biti cist gradijent nego odstupanja.
    nice je deo boje od gradijenta, a displace deo od rand odstupanja*/
    float nice = 0.8;
    float displace = 1 - nice;
    float r, g, b;
    /*s je deo start boje, a e je deo end boje*/
    for (i = 0; i < NUM_BLOCKS; i++) {
        float e = (float) i / NUM_BLOCKS;
        float s = 1 - e;
        if (getColor(blocks[i]) != WHITE) {
            r = start[0] * s + end[0] * e;
            g = start[1] * s + end[1] * e;
            b = start[2] * s + end[2] * e;
            r = r * nice + displace * rand() / (float)(RAND_MAX);
            g = g * nice + displace * rand() / (float)(RAND_MAX);
            b = b * nice + displace * rand() / (float)(RAND_MAX);
            setColor(blocks[i], r, g, b);
        }
    }
}

void initCubes()
{
    int i;
    for (i = 0; i < NUM_BLOCKS; i++) {
        free(blocks[i]);
    }
    NUM_BLOCKS = 0;
    loadBlocks();
}

void drawBullets(void) {
    float x, y, z;
    int i;
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets_active[i]) {
            x = bullets[i].posx, y = bullets[i].posy, z = bullets[i].posz;
            glPushMatrix();
            glTranslatef(x, y, z);
            drawWithColor( & bullets[i]);
            glutSolidSphere(bullets[i].length, 20, 10);
            glPopMatrix();
        }
    }
}

void drawCube(Object* o) {
    drawWithColor(o);
    glPushMatrix();
    glTranslatef(o->posx, o->posy, o->posz);
    glScalef(o->length, o->height, o->width);
    glutSolidCube(1);
    glPopMatrix();
}
