#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include "chess.h"

ButtonStruct SingleplayerButton;

void StartSinglePlayer(){
    CurrentWindow=1;
}

void ShowTransitionAnimation(){
    
}

int currentFrameIndex=0;

int main() {
    InitWindow(WIDTH, HEIGHT, "C Chess");
    SetTargetFPS(60);

    SingleplayerButton = createButton(WIDTH/2-150, HEIGHT/2-25, 50, 300, BLUE, DARKBLUE, &StartSinglePlayer, "Singleplayer");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        Vector2 mouse_pos = GetMousePosition();

        if(CurrentWindow==1){
            for(int i=0; i<8; i++) {
                for(int j=0; j<8; j++) {
                    Color col = ((i + j) % 2 == 0) ? LIGHTGRAY : DARKGRAY;
                    DrawRectangle(i*100, j*100, 100, 100, col);

                }
            }
        }else{
            if(CurrentWindow==0){
                showMenu(WIDTH, HEIGHT, currentFrameIndex);
                drawButton(SingleplayerButton, mouse_pos, 20, BLACK);
            }

            
        }

        


        
        printf("%f - %f\n", mouse_pos.x, mouse_pos.y);

        currentFrameIndex++;

        EndDrawing();
    }
    CloseWindow();
    return 0;
}