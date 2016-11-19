#include "collision.h"

int rangeIntersect(float mina, float maxa, float minb, float maxb);
int hasCollision(Object a, Object b);
int isAbove(Object a, Object b);
Side aRelativeTob(Object a, Object b);
void playerCollision(void);
void bulletCollision(void);

Object cubes[NUM_CUBES];

int rangeIntersect(float mina, float maxa, float minb, float maxb)
{
    /*mozda da proverim da l je proslednjeni min zaista manji od max?*/
    return maxa>=minb && mina<=maxb;
}

/*proverava da li dva objekta imaju presek po svakoj osi,
* ako imaju to je kolizija*/
int hasCollision(Object a, Object b)
{
    float mina,maxa,minb,maxb;
    mina=a.posx-a.length/2;
    maxa=a.posx+a.length/2;
    minb=b.posx-b.length/2;
    maxb=b.posx+b.length/2;
    int interx=rangeIntersect(mina, maxa, minb, maxb);

    mina=a.posy-a.height/2;
    maxa=a.posy+a.height/2;
    minb=b.posy-b.height/2;
    maxb=b.posy+b.height/2;
    int intery=rangeIntersect(mina, maxa, minb, maxb);

    mina=a.posz-a.width/2;
    maxa=a.posz+a.width/2;
    minb=b.posz-b.width/2;
    maxb=b.posz+b.width/2;
    int interz=rangeIntersect(mina, maxa, minb, maxb);

    return interx && intery && interz;
}

int isAbove(Object a, Object b)
{
    return a.posy > b.posy + b.height/2;
}

/*kada ima kolizije da se odluci sa koje strane b je*/
Side aRelativeTob(Object a, Object b)
{
    Side x,z;
    float ax,az,leftx,rightx,frontz,backz;
    ax=a.posx;
    az=a.posz;
    leftx=b.posx-b.length/2;
    rightx=b.posx+b.length/2;
    /*da li je blizi levoj ili desnoj strani*/
    if (fabsf(leftx-ax) < fabsf(rightx-ax)){
        x=LEFT;
        ax=fabsf(leftx-ax);
    }else{
        x=RIGHT;
        ax=fabsf(rightx-ax);
    }
    backz=b.posz-b.width/2;
    frontz=b.posz+b.width/2;
    if (fabsf(backz-az) < fabsf(frontz-az)){
        z=BACK;
        az=fabsf(backz-az);
    }else{
        z=FRONT;
        az=fabsf(frontz-az);
    }
    /*da li je a blizi b po x ili z osi?*/
    printf("%s\n",(ax<az) ? "Blizi x" : "Blizi z");
    return (ax<az) ? x : z;
}


/*GLITCH: iako player nije skroz iznad objekta teleportuje se gore-dole.(napravi max step size)
* Kolizija nekad premesti igraca na pogresnu stranu.
*/
void playerCollision(void)
{
    /*in progress: detekcija kolizije igraca sa okolinom*/
    int i;
    Side side;
    Object p;
    for (i=0;i<NUM_CUBES;i++){
        p=cubes[i];
        if (hasCollision(player,p)){
                /*kolizija sa podom*/
            if (isAbove(player,p)){
                player.posy=p.posy + p.height/2 + player.height/2;
                state.jumping=0;
                player.vy.curr=0;
                /*ako je na plavoj kocki pa odskace visoko*/
                /*TODO funkcija koja prepoznaje boju, potom ovde CASE sta da se radi*/
                if (p.color[0]==0 && p.color[1]==0 && p.color[2]==1){
                    state.bigJump=1;

                }else{
                    state.bigJump=0;
                }
                if(p.color[0]==1 && p.color[1]==1 && p.color[2]==1){
                    state.goFast=1;
                }else{
                    state.goFast=0;
                }
            }else{
                /*ako je kolizija sa strane spreci igraca da ulazi u objekat*/
                side=aRelativeTob(player,p);
                switch(side){
                    case FRONT:
                        player.posz=p.posz+p.width/2+player.width/2;
                        break;
                    case BACK:
                        player.posz=p.posz-p.width/2-player.width/2;
                        break;
                    case LEFT:
                        player.posx=p.posx-p.length/2-player.length/2;
                        break;
                    case RIGHT:
                        player.posx=p.posx+p.length/2+player.length/2;
                        break;
                    default:
                        break;
                }
            }
        }
    }
}

/*prodji kroz sve slotove, ako je aktivan metak proveri koliziju sa kockama*/
void bulletCollision(void)
{
    int i,j;
    for (i=0; i<MAX_BULLETS; i++){
        if (bullets_active[i]){
            for (j=0; j<NUM_CUBES; j++){
                if (hasCollision(cubes[j],bullets[i])){
                    bullets_active[i]=0;
                    setColor(&cubes[j],bullets[i].color[0],bullets[i].color[1],bullets[i].color[2]);
                    /*ako ima kolizije sa kockom ne proveravaj ostale*/
                    break;
                }
            }
        }
    }
}
