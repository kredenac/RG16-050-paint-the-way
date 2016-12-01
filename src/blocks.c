#include "blocks.h"

Object* createBlock();

const float scale = 2;
int NUM_OF_BLOCKS=0;
ObjectNode* Blocks=NULL;

ObjectNode* createNode(float x, float y, float z)
{
    ObjectNode* new= (ObjectNode*) malloc(sizeof(ObjectNode));
    if (new == NULL){
        printf("Greska pri alokaciji novog ObjectNode\n");
        exit(EXIT_FAILURE);
    }
    new->next = NULL;
    Object* obj = createBlock();
    obj->posx = x;
    obj->posy = y;
    obj->posz = z;
    new->o = obj;
    return new;
}

void freeList(ObjectNode** listHead)
{
  ObjectNode* p= NULL;
  /* Ako lista nije prazna, onda treba osloboditi memoriju */
  while (*listHead != NULL) {
    /* Potrebno je prvo zapamtiti adresu sledeceg cvora i onda
       osloboditi cvor koji predstavlja glavu liste */
    p = (*listHead)->next;
    free((*listHead)->o);
    free(*listHead);

    /* Sledeci cvor je nova glava liste */
    *listHead = p;
  }
  NUM_OF_BLOCKS=0;
}

void addToList(ObjectNode** listHead, float x, float y, float z)
{
    ObjectNode* new = createNode(x,y,z);
    new->next = *listHead;
    *listHead = new;
    NUM_OF_BLOCKS++;
}

void removeNode(ObjectNode** listHead, ObjectNode* rm)
{
    /*ako je bas prvi element liste onaj koji treba da se ukloni*/
    if (*listHead == rm ){
        (*listHead)=rm->next;
        free(rm->o);
        free(rm);
        NUM_OF_BLOCKS--;
        return;
    }
    /*inace, kada naidje na cvor ciji next treba da se ukloni,
    tada njegov next postaje next od uklonjenog*/
    ObjectNode* l=*listHead;
    while (l != NULL){
        if (l->next == rm ){
            l->next = rm->next;
            free(rm->o);
            free(rm);
            NUM_OF_BLOCKS--;
            return;
        }
        l = l->next;
    }
}



static float sizex, sizey, sizez;
/*postavlja velicine blokova koji se stvaraju*/
void setSizes(float x, float y, float z)
{
    sizex = x;
    sizey = y;
    sizez = z;
}

Object* createBlock()
{
    Object* novi = (Object*) malloc(sizeof(Object));
    if (novi == NULL){
        printf("Greska pri alokaciji novog Object-a\n");
        exit(EXIT_FAILURE);
    }
    novi->length = sizex;
    novi->height = sizey;
    novi->width = sizez;

    novi->color[0] = rand() / (float)(RAND_MAX);
    novi->color[1] = rand() / (float)(RAND_MAX);
    novi->color[2] = rand() / (float)(RAND_MAX);
    return novi;
}

#define SWAP(x, y, T) do {T SWAP = x; x = y; y = SWAP;} while (0)
/*popunjava kvadar kockama od beg do end po svakoj osi. ako su beg i end
jednaki, onda se stvara samo jedna kocka po toj osi.*/
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
                count++;
                addToList(&Blocks, x, y, z);
            }
        }
    }
    //printf("novi blokovi:%d ukupno:%d\n",count,NUM_BLOCKS);
}

void initBlocks()
{
    freeList(&Blocks);
    loadMap(1);
}
