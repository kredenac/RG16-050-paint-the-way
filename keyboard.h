#ifndef KEYBOARD_H
#define KEYBOARD_H
#define MAX_ELEVATION 89

extern void onReshape(int width, int height);
extern void onKeyboard(unsigned char key, int x, int y);
void onKeyboardUp(unsigned char key, int x, int y);
void onSpecialInput(int key, int x, int y);
void onSpecialInputUp(int key, int x, int y);

extern float aspectRatio;
#endif
