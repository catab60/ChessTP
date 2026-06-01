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

int selectedPieceX = -1;
int selectedPieceY = -1;

Texture2D texPawnWhite;
Texture2D texPawnBlack;
Texture2D texKnightWhite;
Texture2D texKnightBlack;
Texture2D texBishopWhite;
Texture2D texBishopBlack;
Texture2D texRookWhite;
Texture2D texRookBlack;
Texture2D texQueenWhite;
Texture2D texQueenBlack;
Texture2D texKingWhite;
Texture2D texKingBlack;

static Texture2D getPieceTexture(const ChessPiece *p) {
    if (p->piece == 0) return (Texture2D){0};
    bool white = (p->isWhite == 1);
    switch (p->piece) {
        case 1: return white ? texPawnWhite : texPawnBlack;
        case 2: return white ? texKnightWhite : texKnightBlack;
        case 3: return white ? texBishopWhite : texBishopBlack;
        case 4: return white ? texRookWhite : texRookBlack;
        case 5: return white ? texQueenWhite : texQueenBlack;
        case 6: return white ? texKingWhite : texKingBlack;
        default: return (Texture2D){0};
    }
}

static void calculatePossibleMoves(ChessPiece *piece, ChessPiece board[8][8]) {
    piece->numMoves = 0;
    if (piece->piece == 0) return;

    int moveCount = 0;
    int directions[8][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

    if (piece->piece == 1) {
        int direction = (piece->isWhite == 1) ? -1 : 1;
        int nextX = piece->x + direction;
        if (nextX >= 0 && nextX < 8 && board[nextX][piece->y].piece == 0) {
            piece->possibleMoves[moveCount].x = nextX;
            piece->possibleMoves[moveCount].y = piece->y;
            moveCount++;
        }
        for (int dy = -1; dy <= 1; dy += 2) {
            int nextY = piece->y + dy;
            if (nextX >= 0 && nextX < 8 && nextY >= 0 && nextY < 8) {
                if (board[nextX][nextY].piece != 0 && board[nextX][nextY].isWhite != piece->isWhite) {
                    piece->possibleMoves[moveCount].x = nextX;
                    piece->possibleMoves[moveCount].y = nextY;
                    moveCount++;
                }
            }
        }
    }
    else if (piece->piece == 2) {
        int knightMoves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
        for (int i = 0; i < 8; i++) {
            int nx = piece->x + knightMoves[i][0];
            int ny = piece->y + knightMoves[i][1];
            if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
                if (board[nx][ny].piece == 0 || board[nx][ny].isWhite != piece->isWhite) {
                    if (moveCount < 28) {
                        piece->possibleMoves[moveCount].x = nx;
                        piece->possibleMoves[moveCount].y = ny;
                        moveCount++;
                    }
                }
            }
        }
    }
    else if (piece->piece == 3 || piece->piece == 4 || piece->piece == 5) {
        int start = (piece->piece == 2) ? 4 : 0;
        int end = (piece->piece == 4) ? 4 : 8;
        for (int d = start; d < end; d++) {
            for (int dist = 1; dist < 8; dist++) {
                int nx = piece->x + directions[d][0] * dist;
                int ny = piece->y + directions[d][1] * dist;
                if (nx < 0 || nx >= 8 || ny < 0 || ny >= 8) break;
                if (board[nx][ny].piece == 0) {
                    if (moveCount < 28) {
                        piece->possibleMoves[moveCount].x = nx;
                        piece->possibleMoves[moveCount].y = ny;
                        moveCount++;
                    }
                } else {
                    if (board[nx][ny].isWhite != piece->isWhite && moveCount < 28) {
                        piece->possibleMoves[moveCount].x = nx;
                        piece->possibleMoves[moveCount].y = ny;
                        moveCount++;
                    }
                    break;
                }
            }
        }
    }
    else if (piece->piece == 6) {
        for (int d = 0; d < 8; d++) {
            int nx = piece->x + directions[d][0];
            int ny = piece->y + directions[d][1];
            if (nx >= 0 && nx < 8 && ny >= 0 && ny < 8) {
                if (board[nx][ny].piece == 0 || board[nx][ny].isWhite != piece->isWhite) {
                    if (moveCount < 28) {
                        piece->possibleMoves[moveCount].x = nx;
                        piece->possibleMoves[moveCount].y = ny;
                        moveCount++;
                    }
                }
            }
        }
    }

    piece->numMoves = moveCount;
}

void StartSinglePlayer(){

    int pieceContor = 1;


    StartWindowTransition(1);

    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            chessboard[i][j] = createPiece(i, j, 0, 0, -1, chessboard[i][j]);
        }
    }

    for(int i=0;i<8;i++){
        chessboard[1][i]=createPiece(1, i, pieceContor++, 1, 0, chessboard[1][i]);
        chessboard[6][i]=createPiece(6, i, pieceContor++, 1, 1, chessboard[6][i]);
    }

    chessboard[0][0] = createPiece(0,0, pieceContor++, 4, 0, chessboard[0][0]);
    chessboard[0][7] = createPiece(0,7, pieceContor++, 4, 0, chessboard[0][7]);
    chessboard[7][0] = createPiece(7,0, pieceContor++, 4, 1, chessboard[7][0]);
    chessboard[7][7] = createPiece(7,7, pieceContor++, 4, 1, chessboard[7][7]);

    chessboard[0][2] = createPiece(0, 2, pieceContor++, 3, 0, chessboard[0][2]);
    chessboard[0][5] = createPiece(0, 5, pieceContor++, 3, 0, chessboard[0][5]);
    chessboard[7][2] = createPiece(7, 2, pieceContor++, 3, 1, chessboard[7][2]);
    chessboard[7][5] = createPiece(7, 5, pieceContor++, 3, 1, chessboard[7][5]);


    chessboard[0][1] = createPiece(0, 1, pieceContor++, 2, 0, chessboard[0][1]);
    chessboard[0][6] = createPiece(0, 6, pieceContor++, 2, 0, chessboard[0][6]);
    chessboard[7][1] = createPiece(7, 1, pieceContor++, 2, 1, chessboard[7][1]);
    chessboard[7][6] = createPiece(7, 6, pieceContor++, 2, 1, chessboard[7][6]);


    chessboard[0][4] = createPiece(0, 4, pieceContor++, 5, 0, chessboard[0][4]);
    chessboard[7][3] = createPiece(7, 3, pieceContor++, 5, 1, chessboard[7][3]);


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
    MultiplayerButton.x =200;



    MultiplayerButton.col = RED;
}


int currentFrameIndex=0;

int main() {
    InitWindow(WIDTH, HEIGHT, "C Chess");
    SetTargetFPS(60);

    texPawnWhite   = LoadTexture("textures/pawnWhite.png");
    texPawnBlack   = LoadTexture("textures/pawnBlack.png");
    texKnightWhite = LoadTexture("textures/knightWhite.png");
    texKnightBlack = LoadTexture("textures/knightBlack.png");
    texBishopWhite = LoadTexture("textures/bishopWhite.png");
    texBishopBlack = LoadTexture("textures/bishopBlack.png");
    texRookWhite   = LoadTexture("textures/rookWhite.png");
    texRookBlack   = LoadTexture("textures/rookBlack.png");
    texQueenWhite  = LoadTexture("textures/qweenWhite.png");
    texQueenBlack  = LoadTexture("textures/qweenBlack.png");
    texKingWhite   = LoadTexture("textures/kingWhite.png");
    texKingBlack   = LoadTexture("textures/kingBlack.png");

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
                    selectedPieceX = mouseCellX;
                    selectedPieceY = mouseCellY;
                    calculatePossibleMoves(&draggedPiece, chessboard);
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

                bool isValidMove = false;
                for(int m = 0; m < draggedPiece.numMoves; m++) {
                    if(draggedPiece.possibleMoves[m].x == dropY &&
                       draggedPiece.possibleMoves[m].y == dropX) {
                        isValidMove = true;
                        break;
                    }
                }

                if(isValidMove) {
                    draggedPiece.x = dropY;
                    draggedPiece.y = dropX;
                    chessboard[dropY][dropX] = draggedPiece;
                } else {
                    chessboard[dragSourceY][dragSourceX] = draggedPiece;
                }

                dragging = false;
                dragSourceX = -1;
                dragSourceY = -1;
                selectedPieceX = -1;
                selectedPieceY = -1;
            }

            for(int i=0;i<8;i++){
                for(int j=0;j<8;j++){
                    if(chessboard[i][j].piece==0) continue;

                    Texture2D tex = getPieceTexture(&chessboard[i][j]);
                    if (tex.id != 0) {
                        Rectangle src = {0, 0, (float)tex.width, (float)tex.height};
                        Rectangle dest = {(float)j*100.0f, (float)i*100.0f, 100.0f, 100.0f};
                        Vector2 origin = {0, 0};
                        DrawTexturePro(tex, src, dest, origin, 0.0f, WHITE);
                    }
                }
            }

            if(selectedPieceX != -1 && selectedPieceY != -1) {
                for(int m = 0; m < draggedPiece.numMoves; m++) {
                    int moveX = draggedPiece.possibleMoves[m].x; 
                    int moveY = draggedPiece.possibleMoves[m].y; 
                    float centerX = moveY * 100.0f + 50.0f;
                    float centerY = moveX * 100.0f + 50.0f;
                    if(chessboard[moveX][moveY].piece != 0) {
                        DrawCircleLines((int)centerX, (int)centerY, 45, RED);
                        DrawCircleLines((int)centerX, (int)centerY, 44, RED);
                        DrawCircleLines((int)centerX, (int)centerY, 43, RED);
                    } else {
                        DrawCircle((int)centerX, (int)centerY, 15, LIME);
                    }
                }
            }

            if(dragging) {
                Texture2D tex = getPieceTexture(&draggedPiece);
                if (tex.id != 0) {
                    Rectangle src = {0, 0, (float)tex.width, (float)tex.height};
                    Rectangle dest = {mouse_pos.x - 50.0f, mouse_pos.y - 50.0f, 100.0f, 100.0f};
                    Vector2 origin = {0, 0};
                    DrawTexturePro(tex, src, dest, origin, 0.0f, WHITE);
                }
            }

        } else if(CurrentWindow == 0){
            showMenu(WIDTH, HEIGHT, currentFrameIndex);
            drawButton(SingleplayerButton, mouse_pos, 20, BLACK);
            drawButton(MultiplayerButton, mouse_pos, 20, BLACK);
        }else if(CurrentWindow==2){
            printf("multiplayer\n");
        }

        


        

        currentFrameIndex++;

        EndDrawing();
    }

    UnloadTexture(texPawnWhite);
    UnloadTexture(texPawnBlack);
    UnloadTexture(texKnightWhite);
    UnloadTexture(texKnightBlack);
    UnloadTexture(texBishopWhite);
    UnloadTexture(texBishopBlack);
    UnloadTexture(texRookWhite);
    UnloadTexture(texRookBlack);
    UnloadTexture(texQueenWhite);
    UnloadTexture(texQueenBlack);
    UnloadTexture(texKingWhite);
    UnloadTexture(texKingBlack);

    CloseWindow();
    return 0;
}