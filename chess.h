#include "raylib.h"
#include <stdint.h>
extern int WIDTH;
extern int HEIGHT;
extern int CurrentWindow;
extern int TransitionSourceWindow;
extern int TransitionTargetWindow;
extern int TransitionFrameCounter;


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


typedef struct ChessPiece ChessPiece;
struct ChessPiece{
    uint8_t id;
    uint8_t x;
    uint8_t y;
    uint8_t piece; // 1-pion   2-cal   3-nebun   4-tura    5-regina    6-rege    0-empty space
    uint8_t isWhite;
    void (*func)(ChessPiece *piece, Vector2 mouse_pos);
};

ChessPiece createPiece(uint8_t x, uint8_t y, uint8_t id, uint8_t piece, int8_t isWhite, ChessPiece refernace);



void showMenu(int w, int h, int currentFrameIndex);
void ShowTransitionAnimation(int w, int h, int currentFrameIndex);
void StartWindowTransition(int targetWindow);
ButtonStruct createButton(int x, int y, int height, int width, Color col, Color darkcol, void (*func)(), char *btnText);
void drawButton(ButtonStruct button, Vector2 mouse_pos, int fontSize, Color foregound);
