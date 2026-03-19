#include "raylib.h"

extern int WIDTH;
extern int HEIGHT;
extern int CurrentWindow;


typedef struct {
    int x;
    int y;
    int height;
    int width;
    Color col;
    Color darkcol;
    void (*func)();
    char btnText[20];
}ButtonStruct;


void showMenu(int w, int h, int currentFrameIndex);
ButtonStruct createButton(int x, int y, int height, int width, Color col, Color darkcol, void (*func)(), char *btnText);
void drawButton(ButtonStruct button, Vector2 mouse_pos, int fontSize, Color foregound);
