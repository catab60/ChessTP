#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "chess.h"

ButtonStruct SingleplayerButton;
ButtonStruct MultiplayerButton;

ChessPiece chessboard[8][8];
bool dragging = false;
ChessPiece draggedPiece;

int dragSourceX = -1;
int dragSourceY = -1;

void StartSinglePlayer(){

    int pieceContor = 1;


    StartWindowTransition(1);

    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            chessboard[i][j] = createPiece(i, j, 0, 0, -1, chessboard[i][j]);
        }
    }

    //aici punem pioni
    for(int i=0;i<8;i++){
        chessboard[1][i]=createPiece(1, i, pieceContor++, 1, 0, chessboard[1][i]);
        chessboard[6][i]=createPiece(6, i, pieceContor++, 1, 1, chessboard[6][i]);
    }

    // aici punem tura
    chessboard[0][0] = createPiece(0,0, pieceContor++, 4, 0, chessboard[0][0]);
    chessboard[0][7] = createPiece(0,7, pieceContor++, 4, 0, chessboard[0][7]);
    chessboard[7][0] = createPiece(7,0, pieceContor++, 4, 1, chessboard[7][0]);
    chessboard[7][7] = createPiece(7,7, pieceContor++, 4, 1, chessboard[7][7]);

    //aici punem nebuni
    chessboard[0][2] = createPiece(0, 2, pieceContor++, 3, 0, chessboard[0][2]);
    chessboard[0][5] = createPiece(0, 5, pieceContor++, 3, 0, chessboard[0][5]);
    chessboard[7][2] = createPiece(7, 2, pieceContor++, 3, 1, chessboard[7][2]);
    chessboard[7][5] = createPiece(7, 5, pieceContor++, 3, 1, chessboard[7][5]);


    //aici punem cai
    chessboard[0][1] = createPiece(0, 1, pieceContor++, 2, 0, chessboard[0][1]);
    chessboard[0][6] = createPiece(0, 6, pieceContor++, 2, 0, chessboard[0][6]);
    chessboard[7][1] = createPiece(7, 1, pieceContor++, 2, 1, chessboard[7][1]);
    chessboard[7][6] = createPiece(7, 6, pieceContor++, 2, 1, chessboard[7][6]);


    //aici punem reginele
    chessboard[0][4] = createPiece(0, 4, pieceContor++, 5, 0, chessboard[0][4]);
    chessboard[7][3] = createPiece(7, 3, pieceContor++, 5, 1, chessboard[7][3]);


    //aici punem regi
    chessboard[0][3] = createPiece(0, 3, pieceContor++, 6, 0, chessboard[0][3]);
    chessboard[7][4] = createPiece(7, 4, pieceContor++, 6, 1, chessboard[7][4]);





    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            printf("%d ",chessboard[i][j].piece);
        }
        printf("\n");
    }
}


void StartMultiplayer(){
    //StartWindowTransition(2);
    MultiplayerButton.x =200;



    MultiplayerButton.col = RED;
}


int currentFrameIndex=0;

int main() {
    InitWindow(WIDTH, HEIGHT, "C Chess");
    SetTargetFPS(60);

    SingleplayerButton = createButton(WIDTH/2-150, HEIGHT/2-25+50, 50, 300, BLUE, DARKBLUE, &StartSinglePlayer, "Singleplayer");
    MultiplayerButton = createButton(WIDTH/2-150, HEIGHT/2-25+150, 50, 300, BLUE, DARKBLUE, &StartMultiplayer, "Multiplayer");


    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        Vector2 mouse_pos = GetMousePosition();

        if(CurrentWindow == -1){
            int renderWindow = (TransitionFrameCounter < 90) ? TransitionSourceWindow : TransitionTargetWindow;

            if(renderWindow == 0){
                showMenu(WIDTH, HEIGHT, currentFrameIndex);
                drawButton(SingleplayerButton, mouse_pos, 20, BLACK);
                drawButton(MultiplayerButton, mouse_pos, 20, BLACK);
            } else if(renderWindow == 1){
                for(int i=0; i<8; i++) {
                    for(int j=0; j<8; j++) {
                        Color col = ((i + j) % 2 == 0) ? LIGHTGRAY : DARKGRAY;
                        DrawRectangle(i*100, j*100, 100, 100, col);
                    }
                }
            }
\
            ShowTransitionAnimation(WIDTH, HEIGHT, TransitionFrameCounter);

            TransitionFrameCounter++;
            if(TransitionFrameCounter >= 180){
                CurrentWindow = TransitionTargetWindow;
                TransitionFrameCounter = 0;
            }
        } else if(CurrentWindow == 1){
            for(int i=0; i<8; i++) {
                for(int j=0; j<8; j++) {
                    Color col = ((i + j) % 2 == 0) ? LIGHTGRAY : DARKGRAY;
                    DrawRectangle(i*100, j*100, 100, 100, col);
                }
            }
            int mouseCellX = (int)(mouse_pos.x / 100);
            int mouseCellY = (int)(mouse_pos.y / 100);
            if(mouseCellX < 0) mouseCellX = 0;
            if(mouseCellX > 7) mouseCellX = 7;
            if(mouseCellY < 0) mouseCellY = 0;
            if(mouseCellY > 7) mouseCellY = 7;

            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !dragging) {
                if(chessboard[mouseCellY][mouseCellX].piece != 0) {
                    dragging = true;
                    draggedPiece = chessboard[mouseCellY][mouseCellX];
                    dragSourceX = mouseCellX;
                    dragSourceY = mouseCellY;
                    chessboard[dragSourceY][dragSourceX].piece = 0;
                    chessboard[dragSourceY][dragSourceX].isWhite = 0;
                }
            }
            if(dragging && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                int dropX = (int)(mouse_pos.x / 100);
                int dropY = (int)(mouse_pos.y / 100);
                if(dropX < 0) dropX = 0;
                if(dropX > 7) dropX = 7;
                if(dropY < 0) dropY = 0;
                if(dropY > 7) dropY = 7;

                if(chessboard[dropY][dropX].piece == 0) {
                    draggedPiece.x = dropY;
                    draggedPiece.y = dropX;
                    chessboard[dropY][dropX] = draggedPiece;
                } else {
                    chessboard[dragSourceY][dragSourceX] = draggedPiece;
                }

                dragging = false;
                dragSourceX = -1;
                dragSourceY = -1;
            }

            for(int i=0;i<8;i++){
                for(int j=0;j<8;j++){
                    if(chessboard[i][j].piece==0) continue;

                    Color pieceColor = WHITE;
                    switch(chessboard[i][j].piece) {
                        case 1: pieceColor = GREEN; break;
                        case 2: pieceColor = RED; break;
                        case 3: pieceColor = BLUE; break;
                        case 4: pieceColor = ORANGE; break;
                        case 5: pieceColor = PINK; break;
                        case 6: pieceColor = GOLD; break;
                    }

                    DrawRectangle(chessboard[i][j].y*100, chessboard[i][j].x*100, 20, 20, pieceColor);

                    if(chessboard[i][j].isWhite==1){
                        DrawRectangle(chessboard[i][j].y*100, chessboard[i][j].x*100+20, 20, 20, WHITE);
                    } else {
                        DrawRectangle(chessboard[i][j].y*100, chessboard[i][j].x*100+20, 20, 20, BLACK);
                    }
                }
            }

            if(dragging) {
                DrawRectangle(mouse_pos.x - 10, mouse_pos.y - 10, 20, 20, YELLOW);
                if(draggedPiece.isWhite == 1) {
                    DrawRectangle(mouse_pos.x - 10, mouse_pos.y + 10, 20, 20, WHITE);
                } else {
                    DrawRectangle(mouse_pos.x - 10, mouse_pos.y + 10, 20, 20, BLACK);
                }
            }

        } else if(CurrentWindow == 0){
            showMenu(WIDTH, HEIGHT, currentFrameIndex);
            drawButton(SingleplayerButton, mouse_pos, 20, BLACK);
            drawButton(MultiplayerButton, mouse_pos, 20, BLACK);
        }else if(CurrentWindow==2){
            printf("multiplayer\n");
        }

        


        
        //printf("%f - %f\n", mouse_pos.x, mouse_pos.y);

        currentFrameIndex++;

        EndDrawing();
    }
    CloseWindow();
    return 0;
}