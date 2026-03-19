#include "raylib.h"
#include "chess.h"
#include <stdio.h>
#include <string.h>

int WIDTH = 800;
int HEIGHT = 800;
int CurrentWindow = 0;

void showMenu(int w, int h, int currentFrameIndex){

    for(int i=0; i<10; i++) {
        for(int j=0; j<8; j++) {
            Color col = ((i + j) % 2 == 0) ? LIGHTGRAY : DARKGRAY;

            DrawRectangle((i*100+currentFrameIndex)%1000-200,j*100, 100, 100, col);

        }
    }


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