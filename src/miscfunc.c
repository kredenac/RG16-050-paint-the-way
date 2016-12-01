#include "miscfunc.h"
int showFps = 0;

static int isequal(float a, float b);

/*funkcija linearne interpolacije. sluzi da se postepeno
trenutna vrednost priblizava ciljnoj*/
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

/*na osnovu boje postavlja vrednosti 3 float-a*/
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
/*prodje kroz listu i zapise koordinate i velicine objekata*/
void saveMap()
{
    printf("Unesi ime nove mape:\n");
    char name[20];
    scanf("%s",name);
    printf("uneto ime %s\n",name);
    FILE* f = fopen(name,"w");
    if (f == NULL) {
        printf("greska pri pravljenju novog fajla\n");
        exit(EXIT_FAILURE);
    }
    ObjectNode* l;
    float sizex=0, sizey=0, sizez=0;
    int i;
    for (l=Blocks, i=0; l != NULL; l = l->next, i++){
        Object block=*(l->o);
        if (sizex != block.length || sizey != block.height || sizez != block.width){
            sizex = block.length;
            sizey = block.height;
            sizez = block.width;
            fprintf(f, "s %.2f %.2f %.2f\n", sizex, sizey, sizez);
        }
        fprintf(f, "c %.3f %.3f %.3f\n", block.posx / scale, block.posy / scale,
         block.posz / scale);
    }
    fprintf(f, "#Num of blocks: %d\n",i);
    fclose(f);
}

/*Ucitava se fajl koji je sledeceg formata:*/
/*ako linija pocinje sa s - to su duzina, visina i sirina kvadra*/
/*ako linija pocinje sa c - to su koordinate objekta. ostale linije se ignorisu*/
void loadMap(int defaultMap)
{
    char name[20];
    FILE* f;
    if (defaultMap) {
        f = fopen(DEFAULT_MAP, "r");
    } else {
        printf("Koju mapu da loadujem?\n");
        scanf("%s",name);
        f = fopen(name, "r");
    }
    if (f == NULL) {
        printf("greska pri otvaranju .map fajla\n");
        exit(EXIT_FAILURE);
    }
    freeList(&Blocks);
    char line[MAX_LINE];
    int count, i=0;
    float x, y, z;
    float sizex, sizey, sizez;
    while (!feof(f)) {
        fgets(line, MAX_LINE, f);
        i++;
        if (line[0] == 's') {
            count = sscanf(&line[1], "%f %f %f", &sizex, &sizey, &sizez);
            if (count != 3) {
                printf("los .map fajl\n");
                exit(EXIT_FAILURE);
            }
            setSizes(sizex, sizey, sizez);
        } else if (line[0] == 'c') {
            count = sscanf(&line[1], "%f %f %f", &x, &y, &z);
            if (count != 3) {
                printf("los .map fajl, ucitano %d br. linija br %d:\n", count, i);
                printf("%s\n",line);
                exit(EXIT_FAILURE);
            }
            addBlocks(x, x, y, y, z, z);
        }
    }
    fclose(f);
}

void resetGame(void)
{
    resetBullets();
    initBlocks();
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
