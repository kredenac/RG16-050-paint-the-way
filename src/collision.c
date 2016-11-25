#include "collision.h"

int rangeIntersect(float mina, float maxa, float minb, float maxb);
int hasCollision(Object a, Object b);
int isAbove(Object a, Object b);
Side aRelativeTob(Object a, Object b);
void playerCollision(void);
void bulletCollision(void);



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

static float kneeHeight=0.35;
int isAbove(Object a, Object b)
{
    return a.posy - kneeHeight > b.posy + b.height/2;
}

int isBelow(Object a, Object b)
{
    //return a.posy  < b.posy - b.height/2;
    return a.posy - playerHeadHeight + player.height/2 < b.posy - b.height/2;
}

/*kada ima kolizije da se odluci sa koje strane b je*/
Side aRelativeTob(Object a, Object b)
{
    Side x,z;
    float ax,az,leftx,rightx,frontz,backz;
    ax=lastPosx; az=lastPosz;
    leftx=b.posx-b.length/2-a.length/2;
    rightx=b.posx+b.length/2+a.length/2;
    /*da li je blizi levoj ili desnoj strani*/
    if (fabsf(leftx-ax) < fabsf(rightx-ax)){
        x=LEFT;
        ax=fabsf(leftx-ax);
    }else{
        x=RIGHT;
        ax=fabsf(rightx-ax);
    }
    backz=b.posz-b.width/2-a.width/2;
    frontz=b.posz+b.width/2+a.width/2;
    if (fabsf(backz-az) < fabsf(frontz-az)){
        z=BACK;
        az=fabsf(backz-az);
    }else{
        z=FRONT;
        az=fabsf(frontz-az);
    }
    /*da li je a blizi b po x ili z osi?*/
    //printf("%s a.x=%f a.z=%f\n ",(ax<az) ? "Blizi x" : "Blizi z",testx,testz);
    return (ax<az) ? x : z;
}
/*eps sluzi kao mala velicina za koju odaljim igraca od blokova
da ne bi bila kolizija */
static const float eps=0.0001;
void playerCollision(void)
{
    /*postavlja se jumping na 1, pa ako stoji na necemu bice 0*/
    state.jumping=1;
    int i;
    Side side;
    Object p;
    for (i=0;i<NUM_BLOCKS;i++){
        p=*blocks[i];
        if (hasCollision(player,p)){
                /*kolizija sa podom*/
            if (isAbove(player,p)){
                player.posy=p.posy + p.height/2 + player.height/2;
                state.jumping=0;
                player.vy.curr=0;

                Color c=getColor(p);
                switch(c){
                    case(BLUE):
                        state.bigJump=1;
                        state.goFast=0;
                        break;
                    case(ORANGE):
                        state.goFast=1;
                        state.bigJump=0;
                        break;
                    case(WHITE):
                    case(OTHER):
                        state.bigJump=0;
                        state.goFast=0;
                        break;
                }
                /*kolizija sa plafonom*/
            }else if(isBelow(player,p)){
                /*TODO: ulepsaj, mada ok je sad*/
                player.posy-=eps;
                if (player.vy.curr>0){
                    player.vy.curr=-player.vy.curr/2;
                    player.vy.goal=-player.vy.goal/2;
                }
            }else{
                /*ako je kolizija sa strane spreci igraca da ulazi u objekat*/
                side=aRelativeTob(player,p);
                switch(side){
                    case FRONT:
                        player.posz=p.posz+p.width/2+player.width/2+eps;
                        break;
                    case BACK:
                        player.posz=p.posz-p.width/2-player.width/2-eps;
                        break;
                    case LEFT:
                        player.posx=p.posx-p.length/2-player.length/2-eps;
                        break;
                    case RIGHT:
                        player.posx=p.posx+p.length/2+player.length/2+eps;
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
            for (j=0; j<NUM_BLOCKS; j++){
                if (hasCollision(*blocks[j],bullets[i])){
                    bullets_active[i]=0;
                    paintBlock(blocks[j],&bullets[i]);
                    break;
                }
            }
        }
    }
}
