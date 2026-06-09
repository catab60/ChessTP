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
typedef struct {
    uint8_t x;
    uint8_t y;
} Move;
typedef struct ChessPiece ChessPiece;
struct ChessPiece{
    uint8_t id;         
    uint8_t x;          
    uint8_t y;          
    uint8_t piece;      
    uint8_t isWhite;    
    Move possibleMoves[28]; 
    uint8_t numMoves;   
    void (*func)(ChessPiece *piece, Vector2 mouse_pos); 
    uint8_t hasMoved;   
};
ChessPiece createPiece(uint8_t x, uint8_t y, uint8_t id, uint8_t piece, int8_t isWhite, ChessPiece refernace);
void showMenu(int w, int h, int currentFrameIndex);
void ShowTransitionAnimation(int w, int h, int currentFrameIndex);
void StartWindowTransition(int targetWindow);
ButtonStruct createButton(int x, int y, int height, int width, Color col, Color darkcol, void (*func)(), char *btnText);
void drawButton(ButtonStruct button, Vector2 mouse_pos, int fontSize, Color foregound);
