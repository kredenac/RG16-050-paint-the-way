#include "player.h"

const Object playerInit = {
    .posx = 0,
    .posy = 1,
    .posz = 2,
    .vx = {0, 0},
    .vy = {0, 0},
    .vz = {0, 0},
    .length = 0.4,
    .height = 1.4,
    .width = 0.4,
    .color = {0, 0, 0}
};
const Object bulletInit = {
    .posx = 0,
    .posy = 0,
    .posz = 0,
    .vx = {0, 0},
    .vy = {0, 0},
    .vz = {0, 0},
    .length = 0.1,
    .height = 0.1,
    .width = 0.1,
    .color = {1, 1, 1}
};
State state;
const State stateInit = {
    .jumping = 0,
    .fireColor = WHITE,
    .bigJump = 0,
    .goFast = 0,
    .finishedGame = 0,
    .newGame = 1,
    .buildMode=0,
    .flying=0
};

Object player;
extern int dt;

float GRAVITY = -1;
float rotWorld;
Val2f viewAzimuth;
Val2f viewElevation;
float eyex, eyey, eyez;
float lookAtx, lookAty, lookAtz;
float upx, upy, upz;
float playerHeadHeight = 0.25;
float JUMP_V = 0.1;

static float* moveRightCam(void);
static float* moveForwardCam(int yZero);
static int firstFreeLight(void);

/*lastpos - prosla pozicija se pamti zbog racuna pri koliziji*/
float lastPosx, lastPosz, lastPosy;
void movePlayer()
{
    float d = dt / (float) UPDATE_INTERVAL;
    //printf("DISTANCE=%f\n",d);
    /*ako je neko dugme pritisnuto azuriraj brzine*/
    onKeyHold();
    /*jedino ako je igrac u vazduhu ima razloga da ga teram dole*/
    GRAVITY = (state.jumping && !state.flying) ? -1 : 0;
    /*vektori napred i desno relativno od kamere*/
    float* r = moveRightCam();
    float* f = moveForwardCam(1);
    /*brzine postepeno idu ka ciljnim*/
    float step = dt / (float) 5000;
    player.vz.curr = approach(player.vz.goal, player.vz.curr, step);
    player.vx.curr = approach(player.vx.goal, player.vx.curr, step);
    player.vy.curr = approach(player.vy.goal, player.vy.curr, step * 10);
    /*i ciljna brzina opada vremenom ako se ne doda sila na objekat*/
    player.vz.goal = approach(0, player.vz.goal, step / 10);
    player.vx.goal = approach(0, player.vx.goal, step / 10);
    player.vy.goal = approach(GRAVITY, player.vy.goal, step);
    //printf("xcurr%f, xgoal%f, zcurr%f ,zgoal%f\n",player.vx.curr,player.vx.goal,player.vz.curr,player.vz.goal);
    lastPosx = player.posx;
    lastPosz = player.posz;
    lastPosy = player.posy;
    /*pomeraj levo-desno u odnosu na kameru*/
    player.posx += r[0] * player.vx.curr * d;
    player.posz += r[2] * player.vx.curr * d;
    /*pomeraj napred-nazad u odnosu na kameru*/
    player.posz += f[2] * player.vz.curr * d;
    player.posx += f[0] * player.vz.curr * d;
    player.posy += player.vy.curr * d;
}

/*proverava da li se igrac nalazi na mestu gde se triggeruje neki event*/
void checkEvents()
{
    if (player.posy >= 11.6 && player.posy <= 12 && player.posx > -0.8 &&
        player.posx < 0.8 && player.posz <= -17.2 && player.posz >= -18.8) {
        state.finishedGame = 1;
    }
    if (state.finishedGame) {
        psychedelic(600 / dt);
    }
}

/*pomeranje levo-desno u odnosu na kameru*/
static float* moveRightCam(void)
{
    static float v[3];
    float ax = lookAtx - eyex;
    float ay = lookAty - eyey;
    float az = lookAtz - eyez;

    v[0] = ay * upz - upy * az;
    v[1] = 0;
    v[2] = ax * upy - ay * upx;
    normalize3f(v, v + 1, v + 2);
    return v;
}

/*pomeranje napred-nazad u odnosu na  kameru*/
static float* moveForwardCam(int yZero)
{
    static float v[3];
    v[0] = lookAtx - eyex;
    v[1] = yZero != 0 ? 0 : lookAty - eyey;
    v[2] = lookAtz - eyez;
    normalize3f(v, v + 1, v + 2);
    return v;
}

Object bullets[MAX_BULLETS];
int bullets_active[MAX_BULLETS];

#define BULLET_SPEED 5
void firePaint()
{
    int i = 0;
    float r, g, b;
    for (i = 0; i < MAX_BULLETS; i++) {
        /*u prvi neaktivan slot ubaci*/
        if (!bullets_active[i]) {

            bullets_active[i] = 1;
            bullets[i] = bulletInit;
            bullets[i].posx = player.posx;
            bullets[i].posy = player.posy;
            bullets[i].posz = player.posz;
            /*postavljanje boje metka na osnovu izabrane*/
            set3fWithColor(state.fireColor, &r, &g, &b);
            setColor(&bullets[i], r, g, b);
            //printf("pucam %d\n",state.fireColor);
            /*postavljanje vektora brzine metka na tacku gde igrac gleda*/
            float* v = moveForwardCam(0);
            bullets[i].vx.curr = v[0] / BULLET_SPEED;
            bullets[i].vy.curr = v[1] / BULLET_SPEED;
            bullets[i].vz.curr = v[2] / BULLET_SPEED;
            /*izlazi iz petlje jer je postavljen metak*/
            break;
        }
    }

    state.newGame = 0;
}
/*bullets[i].vx.goal koristim kao brojac duzine zivota metaka,
nakon max_bullet_life nestane.*/
static const float MAX_BULLET_LIFE = 1000;
void moveBullets(void)
{
    float d = dt / (float) 17;
    int i;
    for (i = 0; i < MAX_BULLETS; i++)
        if (bullets_active[i]) {
            bullets[i].posx += bullets[i].vx.curr * d;
            bullets[i].posy += bullets[i].vy.curr * d;
            bullets[i].posz += bullets[i].vz.curr * d;
            bullets[i].vx.goal++;
            if (bullets[i].vx.goal > MAX_BULLET_LIFE)
                bullets_active[i] = 0;
        }
}

void resetBullets()
{
    int i;
    for (i = 0; i < MAX_BULLETS; i++) {
        bullets_active[i] = 0;
    }
}

static Object* lightBlock[MAX_LIGHTS];
static int lightAge[MAX_LIGHTS];

void paintBlock(Object* block, Object* bullet)
{
    /*ako treba obojiti beli blok onda iskljuci njegovo svetlo*/
    if (getColor(block) == WHITE) {
        int k;
        for (k = 0; k < MAX_LIGHTS; k++) {
            if (lightBlock[k] == block) {
                lightOn[k] = 0;
                lightAge[k] = 0;
            }
        }
    }
    /*ako treba obojiti blok u belo postavi mu svetlo*/
    if (getColor(bullet) == WHITE) {
        int n = firstFreeLight();
        lightBlock[n] = block;
        lightOn[n] = 1;
        lightAge[n] = glutGet(GLUT_ELAPSED_TIME); //vreme kada je stavljeno
        setLightPos(n, bullet->posx, bullet->posy, bullet->posz);
        psychedelic(1);
    }

    setColor(block, bullet->color[0], bullet->color[1], bullet->color[2]);
}

/*bira se prvi slobodan slot ili najstariji*/
int firstFreeLight(void)
{
    int i;
    /*ne mogu biti starije od trenutnog vremena*/
    int oldest = glutGet(GLUT_ELAPSED_TIME) + 1;
    int oldestIndex;
    for (i = 0; i < MAX_LIGHTS; i++) {
        if (lightOn[i] == 0) {
            return i;
        }
        if (oldest > lightAge[i]) {
            oldest = lightAge[i];
            oldestIndex = i;
        }
    }
    /*Block koji je bio najstariji prestaje da svetli*/
    setColor(lightBlock[oldestIndex], 0, 0, 0);
    return oldestIndex;
}

void jump(void)
{
    if (!state.jumping) {
        player.vy.goal = JUMP_V + state.bigJump * 0.1;
        state.bigJump = 0;
        state.jumping = 1;
    }else if(state.buildMode) {
        state.flying=1;
        player.vy.goal = JUMP_V + state.bigJump * 0.1;
    }
}

void flyDown(void)
{
    if (state.buildMode && state.flying) {
        player.vy.goal = -(JUMP_V + state.bigJump * 0.1);
    }
}

void fireBlackPaint(void)
{
    Color last=state.fireColor;
    state.fireColor=BLACK;
    firePaint();
    state.fireColor=last;
}

void toggleBuildMode(void)
{
    state.buildMode=!state.buildMode;
    printf("%s build mode\n", state.buildMode? "Ukljucen" : "Iskljucen");
    if (!state.buildMode){
        state.flying=0;
    }
}
