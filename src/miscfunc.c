#include "miscfunc.h"
int showFps = 0;

static int isequal(float a, float b);
/*funkcija linearne interpolacije. sluzi da se postepeno menja vrednost*/
float approach(float goal, float curr, float dt)
{
    float diff = goal - curr;
    if (diff > dt) {
        return curr + dt;
    } else if (diff < -dt) {
        return curr - dt;
    }
    /*else je u dometu dt*/
    return goal;
}

void normalize3f(float* x, float* y, float* z)
{
    float n = sqrt(*x * *x +  *y * *y + *z * *z);
    *x = *x / n;
    *y = *y / n;
    *z = *z / n;
}

void setColor(Object* op, float r, float g, float b)
{

    op->color[0] = r;
    op->color[1] = g;
    op->color[2] = b;
}

void set3fWithColor(Color c, float* r1, float* g1, float* b1)
{
    float r, g, b;
    switch (c) {
    case (WHITE):
        r = 1, g = 1, b = 1;
        break;
    case (BLUE):
        r = 0, g = 0, b = 1;
        break;
    case (ORANGE):
        r = 1, g = 0.3, b = 0.1;
        break;
    case (BLACK):
        r=0, g=0, b=0;
        break;
    default:
        r = 1, g = 0.7, b =0.7 ;
        break;
    }
    *r1 = r, *g1 = g, *b1 = b;
}

Color getColor(Object* o)
{
    float col[3];
    col[0] = o->color[0];
    col[1] = o->color[1];
    col[2] = o->color[2];

    if (isequal(col[0], 1) && isequal(col[1], 1) && isequal(col[2], 1)) {
        return WHITE;
    } else if (isequal(col[0], 0) && isequal(col[1], 0) && isequal(col[2], 1)) {
        return BLUE;
    } else if (isequal(col[0], 1) && isequal(col[1], 0.3) && isequal(col[2], 0.1)) {
        return ORANGE;
    } else if (isequal(col[0], 0) && isequal(col[1], 0) && isequal(col[2], 0)){
        return BLACK;
    }

    return OTHER;
}

/*zbog reprezentacije float-a cesto ne budu jednaki kada ih poredim bez castovanja*/
/*takodje sluzi ako ne insistiram da boje moraju biti bas identicne*/
const float eps = 0.01;

int isequal(float a, float b)
{
    if (fabsf(a - b) < eps)
        return 1;
    return 0;
}

#define MAX_LINE 100
/*ucitava koordinate blokova mape iz fajla*/
void loadBlocks(void)
{
    FILE* f;
    f = fopen("src/level.map", "r");
    if (f == NULL) {
        printf("greska pri otvaranju .map fajla\n");
        exit(EXIT_FAILURE);
    }
    char line[MAX_LINE];
    int count;
    float x1, x2, y1, y2, z1, z2;
    float sizex, sizey, sizez;

    while (!feof(f)) {
        fgets(line, MAX_LINE, f);
        if (line[0] == 's') {
            count = sscanf(&line[1], "%f %f %f", &sizex, &sizey, &sizez);
            if (count != 3) {
                printf("los .map fajl\n");
                exit(EXIT_FAILURE);
            }
            setSizes(sizex, sizey, sizez);
        } else if (line[0] != '#') {
            count = sscanf(line, "%f %f %f %f %f %f", &x1, &x2, &y1, &y2, &z1, &z2);
            if (count != 6) {
                printf("los .map fajl %d\n", count);
                exit(EXIT_FAILURE);
            }
            addBlocks(x1, x2, y1, y2, z1, z2);
        }
    }
    fclose(f);
}

void resetGame(void)
{
    resetBullets();
    initCubes();
    initLights();
    state = stateInit;
    player = playerInit;
    viewAzimuth.curr = 270;
    viewElevation.curr = 0;
    eyex = 0, eyey = 1, eyez = 2;
    lookAtx = 0, lookAty = 0, lookAtz = 0;
    upx = 0, upy = 1, upz = 0;
    int i;
    for (i = 0; i < MAX_LIGHTS; i++)
        lightOn[i] = 0;
}
