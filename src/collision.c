#include "collision.h"

int rangeIntersect(float mina, float maxa, float minb, float maxb);
int hasCollision(Object a, Object b);
int isAbove(Object a, Object b);
Side aRelativeTob(Object a, Object b);
void playerCollision(void);
void bulletCollision(void);

int rangeIntersect(float mina, float maxa, float minb, float maxb)
{
    return maxa >= minb && mina <= maxb;
}

/*proverava da li dva objekta imaju presek po svakoj osi,
 * ako imaju to je kolizija*/
int hasCollision(Object a, Object b)
{
    float mina, maxa, minb, maxb;
    mina = a.posx - a.length / 2;
    maxa = a.posx + a.length / 2;
    minb = b.posx - b.length / 2;
    maxb = b.posx + b.length / 2;
    int interx = rangeIntersect(mina, maxa, minb, maxb);

    mina = a.posy - a.height / 2;
    maxa = a.posy + a.height / 2;
    minb = b.posy - b.height / 2;
    maxb = b.posy + b.height / 2;
    int intery = rangeIntersect(mina, maxa, minb, maxb);

    mina = a.posz - a.width / 2;
    maxa = a.posz + a.width / 2;
    minb = b.posz - b.width / 2;
    maxb = b.posz + b.width / 2;
    int interz = rangeIntersect(mina, maxa, minb, maxb);

    return interx && intery && interz;
}

/*da li se objekat a nalazi u objektu b, gledajuci samo xz ravan*/
int aIsInb(Object a, Object b)
{
    float mina, maxa, minb, maxb;
    mina = a.posx - a.length / 4;
    maxa = a.posx + a.length / 4;
    minb = b.posx - b.length / 2;
    maxb = b.posx + b.length / 2;
    int interx = rangeIntersect(mina, maxa, minb, maxb);

    mina = a.posz - a.width / 4;
    maxa = a.posz + a.width / 4;
    minb = b.posz - b.width / 2;
    maxb = b.posz + b.width / 2;
    int interz = rangeIntersect(mina, maxa, minb, maxb);

    return interx && interz;
}

static float kneeHeight = 0.25;
int isAbove(Object a, Object b)
{
    float y=a.posy-a.vy.curr;
    /*ako je isti width kao player smatram da je to bas on*/
    float offset=(a.width==player.width) ? kneeHeight : 0;
    return y - offset > b.posy + b.height / 2;
    // return lastPosy - kneeHeight > b.posy + b.height / 2;
}

int isBelow(Object a, Object b)
{
    if (!aIsInb(a, b))
        return 0;
    float y=a.posy-a.vy.curr;
    /*ako je isti width kao player smatram da je to bas on*/
    float offset=(a.width==player.width) ? playerHeadHeight : 0;
    return y - offset+ a.height / 2 < b.posy - b.height / 2;
    // return lastPosy - playerHeadHeight + player.height / 2 < b.posy - b.height / 2;
}

/*kada ima kolizije da se odluci sa koje strane b je*/
Side aRelativeTob(Object a, Object b)
{
    Side x, z;
    float ax, az, leftx, rightx, frontz, backz;
    ax = a.posx-a.vx.curr;
    az = a.posz-a.vz.curr;
    leftx = b.posx - b.length / 2 - a.length / 2;
    rightx = b.posx + b.length / 2 + a.length / 2;
    /*da li je blizi levoj ili desnoj strani*/
    if (fabsf(leftx - ax) < fabsf(rightx - ax)) {
        x = LEFT;
        ax = fabsf(leftx - ax);
    } else {
        x = RIGHT;
        ax = fabsf(rightx - ax);
    }
    backz = b.posz - b.width / 2 - a.width / 2;
    frontz = b.posz + b.width / 2 + a.width / 2;
    if (fabsf(backz - az) < fabsf(frontz - az)) {
        z = BACK;
        az = fabsf(backz - az);
    } else {
        z = FRONT;
        az = fabsf(frontz - az);
    }
    /*da li je a blizi b po x ili z osi?*/
    //printf("%s a.x=%f a.z=%f\n ",(ax<az) ? "Blizi x" : "Blizi z",testx,testz);
    return (ax < az) ? x : z;
}

/*eps sluzi kao mala velicina za koju odaljim igraca od blokova
da ne bi bila kolizija */
static
const float eps = 0.0001;
void playerCollision(void)
{
    /*postavlja se jumping na 1, pa ako stoji na necemu bice 0*/
    state.jumping = 1;
    Side side;
    Object p;
    /**************************new*******************************/
    ObjectNode* l;
    for (l=Blocks; l!=NULL; l=l->next){
        p=*(l->o);
        /*copy pasta*/
        if (hasCollision(player, p)) {
            /*kolizija sa podom*/
            if (isAbove(player, p)) {
                /*ako je igrac iznad p, ali nije skroz u njemu, ignorise koliziju*/
                if (aIsInb(player, p)) {
                    player.posy = p.posy + p.height / 2 + player.height / 2;
                    state.jumping = 0;
                    state.flying = 0;
                    player.vy.curr = 0;

                    Color c = getColor( & p);
                    switch (c) {
                    case (BLUE):
                        state.bigJump = 1;
                        state.goFast = 0;
                        break;
                    case (ORANGE):
                        state.goFast = 1;
                        state.bigJump = 0;
                        break;
                    case (WHITE):
                    case (BLACK):
                    case (OTHER):
                    default:
                        state.bigJump = 0;
                        state.goFast = 0;
                        break;
                    }
                }
                /*kolizija sa plafonom*/
            } else if (isBelow(player, p)) {
                player.posy -= eps;
                if (player.vy.curr > 0) {
                    player.vy.curr = -player.vy.curr / 2;
                    player.vy.goal = -player.vy.goal / 2;
                }
            } else {
                /*ako je kolizija sa strane spreci igraca da ulazi u objekat*/
                side = aRelativeTob(player, p);
                switch (side) {
                case FRONT:
                    player.posz = p.posz + p.width / 2 + player.width / 2 + eps;
                    break;
                case BACK:
                    player.posz = p.posz - p.width / 2 - player.width / 2 - eps;
                    break;
                case LEFT:
                    player.posx = p.posx - p.length / 2 - player.length / 2 - eps;
                    break;
                case RIGHT:
                    player.posx = p.posx + p.length / 2 + player.length / 2 + eps;
                    break;
                default:
                    break;
                }
            }
        }
    }

    /************************************************************/
}

/*prodji kroz sve slotove, ako je aktivan metak proveri koliziju sa kockama*/
void bulletCollision(void)
{
    int i;
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets_active[i]) {

            /******************************new***********************/
            ObjectNode* l;
            for (l=Blocks; l!=NULL; l=l->next){
                Object p=*(l->o);

                if (hasCollision(p, bullets[i])) {
                    bullets_active[i] = 0;
                    paintBlock(l->o, &bullets[i]);
                    /*********************new***************/
                    if (state.buildMode){
                        Object o=p;//sredi vars
                        /*ako je blok pogodjen crnom bojom onda se brise*/
                        if (getColor(&bullets[i])==BLACK){
                            removeNode(&Blocks, l);
                            printf("removed block\n");
                        }else if (isAbove(bullets[i],o)){
                            addToList(&Blocks,o.posx, o.posy+o.height, o.posz);
                        }else if (isBelow(bullets[i],o)){
                            addToList(&Blocks,o.posx, o.posy-o.height, o.posz);
                        }else{
                            Side side = aRelativeTob(bullets[i], o);
                            float x=o.posx,y=o.posy,z=o.posz;
                            switch (side) {
                            case FRONT:
                                z+=o.width;
                                break;
                            case BACK:
                                z-=o.width;
                                break;
                            case LEFT:
                                x-=o.length;
                                break;
                            case RIGHT:
                                x+=o.length;
                                break;
                            default:
                                break;
                        }

                        addToList(&Blocks, x, y, z);
                        }
                    }
                    /**************************************/
                    break;
                }
            }
            /***********************************************************/
        }
    }
}
