#include "collision.h"

static int rangeIntersect(float mina, float maxa, float minb, float maxb);
static int hasCollision(Object* a, Object* b);
static int isAbove(Object* ap, Object* bp);
static Side aRelativeTob(Object* ap, Object* bp);
static void bulletCollisionBuildMode(Object* bullet, ObjectNode* blockNode);

int rangeIntersect(float mina, float maxa, float minb, float maxb)
{
    return maxa >= minb && mina <= maxb;
}

/*proverava da li dva objekta imaju presek po svakoj osi,
 * ako imaju to je kolizija*/
int hasCollision(Object* ap, Object* bp)
{
    Object a = *ap, b = *bp;
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
int aIsInb(Object* ap, Object* bp)
{
    Object a = *ap, b = *bp;
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
int isAbove(Object* ap, Object* bp)
{
    Object a = *ap, b = *bp;
    float y=a.posy-a.vy.curr;
    /*ako je isti width kao player smatram da je to bas on*/
    float offset=(a.width==player.width) ? kneeHeight : 0;
    return y - offset > b.posy + b.height / 2;
    // return lastPosy - kneeHeight > b.posy + b.height / 2;
}

int isBelow(Object* ap, Object* bp)
{
    Object a = *ap, b = *bp;
    if (!aIsInb(ap, bp))
        return 0;
    float y=a.posy-a.vy.curr;
    /*ako je isti width kao player smatram da je to bas on*/
    float offset=(a.width==player.width) ? playerHeadHeight : 0;
    return y - offset+ a.height / 2 < b.posy - b.height / 2;
    // return lastPosy - playerHeadHeight + player.height / 2 < b.posy - b.height / 2;
}

/*kada ima kolizije da se odluci sa koje strane b je*/
Side aRelativeTob(Object* ap, Object* bp)
{
    Object a = *ap, b = *bp;
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
    ObjectNode* l;
    for (l=Blocks; l!=NULL; l=l->next){
        Object* p = l->o;
        if (hasCollision(&player, p)) {
            /*kolizija sa podom*/
            if (isAbove(&player, p)) {
                /*ako je igrac iznad p, ali nije skroz u njemu, ignorise koliziju*/
                if (aIsInb(&player, p)) {
                    playerOnBlockReact(p);
                }
                /*kolizija sa plafonom*/
            } else if (isBelow(&player, p)) {
                player.posy -= eps;
                if (player.vy.curr > 0) {
                    player.vy.curr = -player.vy.curr / 2;
                    player.vy.goal = -player.vy.goal / 2;
                }
            } else {
                /*ako je kolizija sa strane spreci igraca da ulazi u objekat*/
                Side side = aRelativeTob(&player, p);
                switch (side) {
                case FRONT:
                    player.posz = p->posz + p->width / 2 + player.width / 2 + eps;
                    break;
                case BACK:
                    player.posz = p->posz - p->width / 2 - player.width / 2 - eps;
                    break;
                case LEFT:
                    player.posx = p->posx - p->length / 2 - player.length / 2 - eps;
                    break;
                case RIGHT:
                    player.posx = p->posx + p->length / 2 + player.length / 2 + eps;
                    break;
                default:
                    break;
                }
            }
        }
    }
}

/*prolazi kroz sve slotove, ako je aktivan metak proveri koliziju sa kockama*/
void bulletCollision(void)
{
    int i;
    for (i = 0; i < MAX_BULLETS; i++) {
        if (bullets_active[i]) {
            ObjectNode* l;
            for (l = Blocks; l != NULL; l = l->next){
                Object* block = l->o;

                if (hasCollision(block, &bullets[i])) {
                    bullets_active[i] = 0;
                    paintBlock(block, &bullets[i]);
                    bulletCollisionBuildMode(&bullets[i], l);
                    break;
                }
            }
        }
    }
}

void bulletCollisionBuildMode(Object* bullet, ObjectNode* blockNode)
{
    if (!state.buildMode)
        return;
    Object* block = blockNode->o;
    /*ako je blok pogodjen crnom bojom onda se brise*/
    if (getColor(bullet) == BLACK){
        removeNode(&Blocks, blockNode);
    /*inace napravi novi blok sa one strane bloka gde je udario metak*/
    }else if (isAbove(bullet, block)){
        addToList(&Blocks, block->posx, block->posy + block->height, block->posz);

    }else if (isBelow(bullet, block)){
        addToList(&Blocks, block->posx, block->posy - block->height, block->posz);

    }else{
        Side side = aRelativeTob(bullet, block);
        float x = block->posx, y = block->posy, z = block->posz;
        switch (side) {
        case FRONT:
            z += block->width;
            break;
        case BACK:
            z -= block->width;
            break;
        case LEFT:
            x -= block->length;
            break;
        case RIGHT:
            x += block->length;
            break;
        default:
            break;
        }
        addToList(&Blocks, x, y, z);
    }
}
