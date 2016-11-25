#include "player.h"

const Object playerInit={
    .posx=0, .posy=1, .posz=2,
    .vx={0,0}, .vy={0.0}, .vz={0,0},
    .length=0.4, .height=1.4, .width=0.4,
    .color={0,0,0}
};
const Object bulletInit={
    .posx=0, .posy=0, .posz=0,
    .vx={0,0}, .vy={0.0}, .vz={0,0},
    .length=0.1, .height=0.1, .width=0.1,
    .color={1,1,1}
};
State state;
const State stateInit={
    .jumping=0,
    .fireColor=0,
    .bigJump=0,
    .goFast=0,
    .finishedGame=0
};

Object player;
extern int dt;

float GRAVITY=-1;
float rotWorld;
Val2f viewAzimuth;
Val2f viewElevation;
float eyex, eyey, eyez;
float lookAtx, lookAty, lookAtz;
float upx, upy, upz;
float playerHeadHeight=0.25;
float JUMP_V=0.1;

static float* moveRightCam(void);
static float* moveForwardCam(int yZero);
static void checkEvents(void);
static int firstFreeLight(void);


/*lastpos - prosla pozicija se pamti zbog racuna pri koliziji*/
float lastPosx, lastPosz;
void movePlayer(){
    /*ako je neko dugme pritisnuto azuriraj brzine*/
    onKeyHold();
    /*jedino ako je igrac u vazduhu ima razloga da ga teram dole*/
    GRAVITY=state.jumping ? -1 : 0;
    /*vektori napred i desno relativno od kamere*/
    float * r=moveRightCam();
    float * f=moveForwardCam(1);
    /*brzine postepeno idu ka ciljnim*/
    player.vz.curr=approach(player.vz.goal, player.vz.curr, dt/(float)5000);
    player.vx.curr=approach(player.vx.goal, player.vx.curr, dt/(float)5000);
    player.vy.curr=approach(player.vy.goal, player.vy.curr, dt/(float)500);
    /*i ciljna brzina opada vremenom ako se ne doda sila na objekat*/
    player.vz.goal=approach(0, player.vz.goal, dt/(float)50000);
    player.vx.goal=approach(0, player.vx.goal, dt/(float)50000);
    player.vy.goal=approach(GRAVITY, player.vy.goal, dt/(float)5000);
    //printf("xcurr%f, xgoal%f, zcurr%f ,zgoal%f\n",player.vx.curr,player.vx.goal,player.vz.curr,player.vz.goal);
    lastPosx=player.posx;
    lastPosz=player.posz;
    /*pomeraj levo-desno u odnosu na kameru*/
    player.posx+=r[0]*player.vx.curr;
    player.posz+=r[2]*player.vx.curr;
    /*pomeraj napred-nazad u odnosu na kameru*/
    player.posz+=f[2]*player.vz.curr;
    player.posx+=f[0]*player.vz.curr;
    player.posy+=player.vy.curr;
    checkEvents();
}

/*proverava da li se igrac nalazi na mestu gde se triggeruje neki event*/
void checkEvents()
{
    if (player.posy>=11.6 && player.posy<=12 && player.posx>-0.8 && player.posx<0.8 && player.posz<=-17.2 && player.posz>=-18.8){
        state.finishedGame=1;
    }
}

/*pomeranje levo-desno u odnosu na kameru*/
static float* moveRightCam(void)
{

    static float v[3];
    float ax=lookAtx-eyex;
    float ay=lookAty-eyey;
    float az=lookAtz-eyez;

    v[0]=ay*upz -upy*az;
    v[1]=0;
    v[2]=ax*upy - ay*upx;
    normalize3f(v,v+1,v+2);
    return v;
}

/*pomeranje napred-nazad u odnosu na  kameru*/
static float* moveForwardCam(int yZero)
{

    static float v[3];
    v[0]=lookAtx-eyex;
    v[1]=yZero!=0 ? 0 :lookAty-eyey;
    v[2]=lookAtz-eyez;
    normalize3f(v,v+1,v+2);
    return v;
}


Object bullets[MAX_BULLETS];
int bullets_active[MAX_BULLETS];

#define BULLET_SPEED 10
void firePaint()
{
    int i=0;
    float r,g,b;
    for (i=0;i<MAX_BULLETS;i++){
        /*u prvi neaktivan slot ubaci*/
        if (!bullets_active[i]){
            bullets_active[i]=1;
            bullets[i]=bulletInit;
            bullets[i].posx=player.posx;
            bullets[i].posy=player.posy;
            bullets[i].posz=player.posz;
            /*postavljanje boje metka na osnovu izabrane*/
            set3fWithColor(state.fireColor,&r,&g,&b);
            setColor(&bullets[i], r, g, b);
            /*postavljanje vektora brzine metka na tacku gde igrac gleda*/
            float* v=moveForwardCam(0);
            bullets[i].vx.curr=v[0]/BULLET_SPEED;
            bullets[i].vy.curr=v[1]/BULLET_SPEED;
            bullets[i].vz.curr=v[2]/BULLET_SPEED;
            /*izlazi iz petlje jer je postavljen metak*/
            break;
        }
    }
}
/*bullets[i].vx.goal koristim kao brojac duzine zivota metaka
nakon max_bullet_life nestane.*/
static const float MAX_BULLET_LIFE=1000;
void moveBullets(void)
{
    int i;
    for (i=0;i<MAX_BULLETS;i++)
        if (bullets_active[i]){
            bullets[i].posx+=bullets[i].vx.curr;
            bullets[i].posy+=bullets[i].vy.curr;
            bullets[i].posz+=bullets[i].vz.curr;
            bullets[i].vx.goal++;
            if (bullets[i].vx.goal>MAX_BULLET_LIFE)
                bullets_active[i]=0;
    }
}

void resetBullets()
{
    int i;
    for (i=0;i<MAX_BULLETS;i++){
        bullets_active[i]=0;
    }
}

static Object* lightBlock[MAX_LIGHTS];
static int lightAge[MAX_LIGHTS];

void paintBlock(Object* block, Object* bullet)
{
    /*ako treba obojiti beli blok onda iskljuci njegovo svetlo*/
    if (getColor(*block)==WHITE){
        int k;
        for (k=0; k<MAX_LIGHTS; k++){
            if (lightBlock[k]==block){
                lightOn[k]=0;
                lightAge[k]=0;
            }
        }
    }
    /*ako treba obojiti blok u belo postavi mu svetlo*/
    if (getColor(*bullet)==WHITE){
        int n=firstFreeLight();
        lightBlock[n]=block;
        lightOn[n]=1;
        lightAge[n]=glutGet(GLUT_ELAPSED_TIME);//vreme kada je stavljeno
        setLightPos(n, bullet->posx, bullets->posy, bullets->posz);
        psychedelic(1);
    }
    setColor(block,bullets->color[0],bullet->color[1],bullets->color[2]);
}

/*bira se prvi slobodan slot ili najstariji*/
int firstFreeLight(void)
{
    int i;
    /*ne mogu biti starije od trenutnog vremena*/
    int oldest=glutGet(GLUT_ELAPSED_TIME)+1;
    int oldestIndex;
    for (i=0; i<MAX_LIGHTS; i++){
        if (lightOn[i]==0){
            return i;
        }
        if (oldest > lightAge[i]){
            oldest=lightAge[i];
            oldestIndex=i;
        }
    }
    /*Block koji je bio najstariji prestaje da svetli*/
    setColor(lightBlock[oldestIndex], 0, 0, 0);
    return oldestIndex;
}
