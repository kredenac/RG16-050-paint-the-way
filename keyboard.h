#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <GL/glut.h>
#include "player.h"
#include "miscfunc.h"

extern void onReshape(int width, int height);
extern void onKeyboard(unsigned char key, int x, int y);
extern void onKeyboardUp(unsigned char key, int x, int y);
extern void onSpecialInput(int key, int x, int y);
extern void onSpecialInputUp(int key, int x, int y);
extern void mouseLook(int x, int y);
extern float aspectRatio;
#endif
