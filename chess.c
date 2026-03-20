#include "raylib.h"
#include "chess.h"
#include <stdio.h>
#include <string.h>

int WIDTH = 800;
int HEIGHT = 800;
int CurrentWindow = 0;
int TransitionSourceWindow = 0;
int TransitionTargetWindow = 0;
int TransitionFrameCounter = 0;

void StartWindowTransition(int targetWindow){
    TransitionSourceWindow = CurrentWindow;
    TransitionTargetWindow = targetWindow;
    CurrentWindow = -1;
    TransitionFrameCounter = 0;
}

void ClickPiece(ChessPiece *piece, Vector2 mouse_pos){
    if (!piece) return;
    int nx = (int)(mouse_pos.y);
    int ny = (int)(mouse_pos.x);
    if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
        piece->x = nx;
        piece->y = ny;
    }
}

ChessPiece createPiece(uint8_t x, uint8_t y, uint8_t id, uint8_t piece, int8_t isWhite, ChessPiece referance){
    ChessPiece temp;
    temp.piece = piece;
    temp.id = id;
    temp.x = x;
    temp.y = y;
    temp.isWhite = isWhite;
    temp.func = &ClickPiece;
    return temp;
}

void showMenu(int w, int h, int currentFrameIndex){

    for(int i=0; i<10; i++) {
        for(int j=0; j<8; j++) {
            Color col = ((i + j) % 2 == 0) ? LIGHTGRAY : DARKGRAY;
            DrawRectangle((i*100+currentFrameIndex)%1000-200,j*100, 100, 100, col);
        }
    }
}


void ShowTransitionAnimation(int w, int h, int currentFrameIndex){
    // Fade from 0 -> 255 -> 0 over a cycle
    const int cycleFrames = 180; // 3 seconds at 60 FPS
    int frame = currentFrameIndex % cycleFrames;
    float t = (float)frame / (float)(cycleFrames - 1);
    float alphaFactor = (t <= 0.5f) ? (t * 2.0f) : (2.0f - t * 2.0f);
    unsigned char alpha = (unsigned char)(alphaFactor * 255.0f);

    DrawRectangle(0, 0, w, h, (Color){0, 0, 0, alpha});
}

ButtonStruct createButton(int x, int y, int height, int width, Color col, Color darkcol, void (*func)(), char *btnText){


        ButtonStruct btn;
        btn.x = x;
        btn.y = y;
        btn.width = width;
        btn.height = height;
        btn.col = col;
        btn.darkcol = darkcol;
        btn.func = func;
        strcpy(btn.btnText, btnText);


    return btn;


}

void drawButton(ButtonStruct button, Vector2 mouse_pos, int fontSize, Color foregound){


    if(mouse_pos.x>=button.x && mouse_pos.x<=button.x + button.width){
        if(mouse_pos.y>=button.y && mouse_pos.y<=button.y+button.height){
            DrawRectangle(button.x, button.y, button.width, button.height, button.darkcol);
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                if(button.func!=NULL){
                    button.func();
                }
            }
        }else{
            DrawRectangle(button.x, button.y, button.width, button.height, button.col);
        }
    }else{
        DrawRectangle(button.x, button.y, button.width, button.height, button.col);
    }

    // Center text inside the button
    Vector2 textSize = MeasureTextEx(GetFontDefault(), button.btnText, (float)fontSize, 10);
    float textX = button.x + (button.width - textSize.x) * 0.5f;
    float textY = button.y + (button.height - textSize.y) * 0.5f;
    DrawTextEx(GetFontDefault(), button.btnText, (Vector2){textX, textY}, (float)fontSize, 10, foregound);




}