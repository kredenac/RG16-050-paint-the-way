#ifndef BLOCKS_H
#define BLOCKS_H
#include <stdio.h>
#include <stdlib.h>
#include "player.h"

typedef struct ObjectNode{
    struct ObjectNode* next;
    Object* o;
}ObjectNode;

extern void addBlocks(float begx, float endx, float begy, float endy, float begz, float endz);
extern void setSizes(float x, float y, float z);
extern void initCubes(void);
extern void addToList(ObjectNode** listHead, float x, float y, float z);
extern void removeNode(ObjectNode** listHead, ObjectNode* rm);
extern int NUM_OF_BLOCKS;

extern ObjectNode* Blocks;


#endif
