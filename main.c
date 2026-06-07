#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "chess.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>

ButtonStruct SingleplayerButton;
ButtonStruct MultiplayerButton;
ButtonStruct CreateGameButton;
ButtonStruct JoinGameButton;
ButtonStruct LobbyBackButton;
ButtonStruct JoinConnectButton;
bool isTheGameInMultiplayerMode=false;
int stockfishDifficultyLevelNumber = 1;

// astea is pentru stockfish (e o nenoricire)
int theSocketForTheClient = -1;
int theSocketForTheServer = -1;
int theLobbyCodeNumber = 0;
bool isTheHostOfGame = false;
bool isTheConnectionEstablishedNow = false;
int playerColorNumberWhiteIs1BlackIs0 = 1; // 1 pentru alb, 0 pt negru
char stringForLobbyCodeInput[16] = "";
int lengthOfLobbyCodeInput = 0;
bool didTheConnectionFail = false;
char stringForConnectionErrorMessage[64] = "";




ChessPiece chessboard[8][8];
bool dragging=false;
ChessPiece draggedPiece;
int isWhiteTurn=1;
bool gameOver= false;
int winnerColor=-1;
float gameOverTimer=0.0f;

int dragSourceX =-1;
int dragSourceY =-1;





int selectedPieceX =-1;
int selectedPieceY =-1;

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







static Texture2D getPieceTexture(const ChessPiece *p){
    if(p->piece == 0) return (Texture2D){0};
    bool white = (p->isWhite == 1);
    switch(p->piece){
        case 1: return white ? texPawnWhite : texPawnBlack;
        case 2: return white ? texKnightWhite : texKnightBlack;
        case 3: return white ? texBishopWhite : texBishopBlack;
        case 4: return white ? texRookWhite : texRookBlack;
        case 5: return white ? texQueenWhite : texQueenBlack;
        case 6: return white ? texKingWhite : texKingBlack;
        default: return (Texture2D){0};
    }
}

static void calculatePossibleMoves(ChessPiece *piece, ChessPiece board[8][8]){
    piece->numMoves = 0;
    if(piece->piece == 0) return;

    int moveCount = 0;
    int directions[8][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

    if(piece->piece == 1){
        int direction = (piece->isWhite == 1) ? -1 : 1;
        int nextX = piece->x + direction;

        if(nextX >= 0 && nextX < 8 && board[nextX][piece->y].piece == 0){
            piece->possibleMoves[moveCount].x = nextX;
            piece->possibleMoves[moveCount].y = piece->y;
            moveCount++;



            int startRow=(piece->isWhite==1)?6:1;
            if(piece->x==startRow){
                int nextX2 =piece->x + 2*direction;
                if(nextX2 >=0&&nextX2<8&&board[nextX2][piece->y].piece == 0){
                    piece->possibleMoves[moveCount].x=nextX2;
                    piece->possibleMoves[moveCount].y=piece->y;
                    moveCount++;
                }

            }
        }






        for(int dy = -1; dy <= 1; dy += 2){
            int nextY = piece->y + dy;
            if(nextX >= 0 && nextX < 8 && nextY >= 0 && nextY < 8){
                if(board[nextX][nextY].piece != 0 && board[nextX][nextY].isWhite != piece->isWhite){

                    piece->possibleMoves[moveCount].x = nextX;
                    piece->possibleMoves[moveCount].y = nextY;
                    moveCount++;
                }
            }
        }
    }
    else if(piece->piece == 2){
        int knightMoves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
        for(int i = 0; i < 8; i++){
            int nx = piece->x + knightMoves[i][0];

            int ny = piece->y + knightMoves[i][1];
            if(nx >= 0 && nx < 8 && ny >= 0 && ny < 8){
                if(board[nx][ny].piece == 0 || board[nx][ny].isWhite != piece->isWhite){
                    if(moveCount < 28){
                        piece->possibleMoves[moveCount].x = nx;
                        piece->possibleMoves[moveCount].y = ny;
                        moveCount++;
        
                    }
                }
            }


        }
    }
    else if(piece->piece == 3 || piece->piece == 4 || piece->piece == 5){
        int start = (piece->piece == 3) ? 4 : 0;
        int end = (piece->piece == 4) ? 4 : 8;
        for(int d = start; d < end; d++){
            for(int dist = 1; dist < 8; dist++){

                int nx = piece->x + directions[d][0] * dist;
                int ny = piece->y + directions[d][1] * dist;
                if(nx < 0 || nx >= 8 || ny < 0 || ny >= 8) break;
                if(board[nx][ny].piece == 0){
                    if(moveCount < 28){
                        piece->possibleMoves[moveCount].x = nx;
                        piece->possibleMoves[moveCount].y = ny;
                        moveCount++;
                    }
                } else {
                    if(board[nx][ny].isWhite != piece->isWhite && moveCount < 28){
                        piece->possibleMoves[moveCount].x = nx;
                        piece->possibleMoves[moveCount].y = ny;
                        moveCount++;
                    }
                    break;
                }

            }
        }
    }
    else if(piece->piece == 6){
        for(int d = 0; d < 8; d++){
            int nx = piece->x + directions[d][0];
            int ny = piece->y + directions[d][1];
            if(nx >= 0 && nx < 8 && ny >= 0 && ny < 8){
                if(board[nx][ny].piece == 0 || board[nx][ny].isWhite != piece->isWhite){
                    if(moveCount < 28){
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








static void findKing(int whiteKing, const ChessPiece board[8][8], int *kx, int *ky){
    *kx=-1;
    *ky=-1;
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(board[i][j].piece==6&&board[i][j].isWhite==whiteKing){
                *kx=i;
                *ky=j;
                return;
            }
        }
    }
}



static bool isSquareAttacked(int friendlyColor, int x, int y, ChessPiece board[8][8]){
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(board[i][j].piece!=0&&board[i][j].isWhite!=friendlyColor){
                ChessPiece tempPiece=board[i][j];
                calculatePossibleMoves(&tempPiece,board);
                for(int m=0;m<tempPiece.numMoves;m++){
                    if(tempPiece.possibleMoves[m].x==x&&tempPiece.possibleMoves[m].y==y){
                        return true;
                    }
                }
            }
        }
    }
    return false;
}


static bool isKingInCheck(int whiteKing, ChessPiece board[8][8]){
    int kx=-1, ky=-1;
    findKing(whiteKing, board, &kx, &ky);
    if(kx==-1||ky==-1){
        return false;
    }


    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(board[i][j].piece!=0&&board[i][j].isWhite!=whiteKing){
                ChessPiece tempPiece=board[i][j];
                calculatePossibleMoves(&tempPiece, board);
                for(int m=0;m<tempPiece.numMoves;m++){
                    if(tempPiece.possibleMoves[m].x==kx&&tempPiece.possibleMoves[m].y==ky){
                        return true;
                    }
                }


            }
        }
    }
    return false;
}

static void filterIllegalMoves(ChessPiece *piece, ChessPiece board[8][8]){
    Move legalMoves[28];
    int legalCount=0;



    for(int m=0;m<piece->numMoves;m++){
        ChessPiece tempBoard[8][8];
        memcpy(tempBoard, board, sizeof(ChessPiece)*8*8);

        int destX=piece->possibleMoves[m].x;
        int destY=piece->possibleMoves[m].y;

        tempBoard[piece->x][piece->y]=createPiece(piece->x, piece->y, 0, 0, -1, tempBoard[piece->x][piece->y]);


        tempBoard[destX][destY]= *piece;
        tempBoard[destX][destY].x=destX;
        tempBoard[destX][destY].y=destY;

        if(!isKingInCheck(piece->isWhite, tempBoard)){
            legalMoves[legalCount]=piece->possibleMoves[m];
            legalCount++;
        }
    }

    piece->numMoves=legalCount;
    memcpy(piece->possibleMoves, legalMoves, sizeof(Move)*legalCount);
}



static bool hasAnyLegalMoves(int whitePlayer, ChessPiece board[8][8]){
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(board[i][j].piece!=0&&board[i][j].isWhite==whitePlayer){
                ChessPiece tempPiece=board[i][j];
                calculatePossibleMoves(&tempPiece, board);
                filterIllegalMoves(&tempPiece, board);
                if(tempPiece.numMoves>0){
                    return true;
                }
            }
        }
    }
    return false;
}


//aici doar eu si dumnezeu stiu ce am facut
static void calculateCastlingMoves(ChessPiece *piece, ChessPiece board[8][8]){
    if(piece->piece!=6||piece->hasMoved) return;

    int moveCount =piece->numMoves;

    if(piece->isWhite==1){
        if(piece->x==7&&piece->y==4){
            if(board[7][7].piece==4&&board[7][7].isWhite==1&&!board[7][7].hasMoved){
                if(board[7][5].piece==0&&board[7][6].piece==0){
                    if(!isKingInCheck(1, board)){
                        if(!isSquareAttacked(1, 7, 5, board)&&!isSquareAttacked(1, 7, 6, board)){
                            if(moveCount<28){


                                piece->possibleMoves[moveCount].x=7;
                                piece->possibleMoves[moveCount].y=6;
                                moveCount++;
                            }
                        }
                    }
                }
            }




            if(board[7][0].piece==4&&board[7][0].isWhite==1&&!board[7][0].hasMoved){
                if(board[7][1].piece==0&&board[7][2].piece==0&&board[7][3].piece==0){
                    if(!isKingInCheck(1, board)){
                        if(!isSquareAttacked(1, 7, 2, board)&&!isSquareAttacked(1, 7, 3, board)){
                            if(moveCount<28){
                                piece->possibleMoves[moveCount].x=7;
                                piece->possibleMoves[moveCount].y=2;
                                moveCount++;
                            }
                        }

                    }
                }

            }
        }
    }else{
        if(piece->x==0&&piece->y==4){
            if(board[0][7].piece==4&&board[0][7].isWhite==0&&!board[0][7].hasMoved){
                if(board[0][5].piece==0&&board[0][6].piece==0){
                    if(!isKingInCheck(0, board)){

                        if(!isSquareAttacked(0, 0, 5, board)&&!isSquareAttacked(0, 0, 6, board)){
                            if(moveCount<28){
                                piece->possibleMoves[moveCount].x=0;
                                piece->possibleMoves[moveCount].y=6;
                                moveCount++;
                            }
                        }
                    }

                }
            }
            if(board[0][0].piece==4&&board[0][0].isWhite==0&&!board[0][0].hasMoved){
                if(board[0][1].piece==0&&board[0][2].piece==0&&board[0][3].piece==0){
                    if(!isKingInCheck(0, board)){
                        if(!isSquareAttacked(0, 0, 2, board)&&!isSquareAttacked(0, 0, 3, board)){
                            if(moveCount<28){
                                piece->possibleMoves[moveCount].x=0;
                                piece->possibleMoves[moveCount].y=2;
                                moveCount++;
                            }
                        }
                    }
                }

            }
        }
    }

    piece->numMoves=moveCount;
}




static void generateFEN(char *fen, ChessPiece board[8][8], int activeColor){

    int pos=0;
    for(int r=0;r<8;r++){
        int emptyCount=0;
        for(int c=0;c<8;c++){
            if(board[r][c].piece==0){
                emptyCount++;
            }else{
                if(emptyCount>0){
                    pos+=sprintf(fen+pos, "%d", emptyCount);
                    emptyCount=0;
                }
                char pChar=' ';
                switch(board[r][c].piece){
                    case 1: pChar='p'; break;
                    case 2: pChar='n'; break;
                    case 3: pChar='b'; break;
                    case 4: pChar='r'; break;
                    case 5: pChar='q'; break;
                    case 6: pChar='k'; break;
                }
                if(board[r][c].isWhite==1){
                    pChar= pChar-'a'+'A';
                }
                fen[pos++]=pChar;
            }
        }
        if(emptyCount>0){
            pos+= sprintf(fen+pos, "%d", emptyCount);
        }
        if(r<7){
            fen[pos++]='/';
        }

    }

    pos+=sprintf(fen+pos, " %c", (activeColor==1)?'w':'b');

    pos+= sprintf(fen+pos, " ");
    int castlingPos= pos;
    if(board[7][4].piece==6&&board[7][4].isWhite==1&&!board[7][4].hasMoved){
        if(board[7][7].piece==4&&board[7][7].isWhite==1&&!board[7][7].hasMoved){
            fen[pos++]='K';
        }
        if(board[7][0].piece==4&&board[7][0].isWhite==1&&!board[7][0].hasMoved){
            fen[pos++]='Q';
        }
    }
    if(board[0][4].piece==6&&board[0][4].isWhite==0&&!board[0][4].hasMoved){
        if(board[0][7].piece==4&&board[0][7].isWhite==0&&!board[0][7].hasMoved){
            fen[pos++]='k';
        }
        if(board[0][0].piece==4&&board[0][0].isWhite==0&&!board[0][0].hasMoved){
            fen[pos++]='q';
        }
    }
    if(pos==castlingPos){
        fen[pos++]='-';
    }

    pos+= sprintf(fen+pos, " - 0 1");
    fen[pos]='\0';
}

static void getStockfishMove(char *bestMove, const char *fen, int skillLevel){
    int writePipe[2];
    int readPipe[2];

    if(pipe(writePipe)<0||pipe(readPipe)<0){
        return;
    }
    pid_t pid=fork();
    if(pid<0){
        return;
    }
    if(pid==0){
        dup2(writePipe[0], STDIN_FILENO);
        dup2(readPipe[1], STDOUT_FILENO);

        close(writePipe[1]);
        close(readPipe[0]);

        char *args[] = {"stockfish", NULL};
        execvp("stockfish", args);
        exit(1);
    }else{
        close(writePipe[0]);
        close(readPipe[1]);

        char command[512];
        sprintf(command, "uci\nisready\nsetoption name Skill Level value %d\nposition fen %s\ngo movetime 100\nquit\n", skillLevel, fen);
        
        int totalWritten=0;
        int cmdLen=strlen(command);
        while(totalWritten<cmdLen){
            int written=write(writePipe[1], command+totalWritten, cmdLen-totalWritten);
            if(written<=0) break;
            totalWritten+=written;
        }
        close(writePipe[1]);

        char buffer[1024];
        int bytesRead;
        char response[8192]={0};
        int responseLen=0;

        while((bytesRead=read(readPipe[0], buffer, sizeof(buffer)-1))>0){
            buffer[bytesRead]='\0';
            if(responseLen+bytesRead<sizeof(response)){
                strcat(response, buffer);
                responseLen+=bytesRead;
            }
        }
        close(readPipe[0]);
        waitpid(pid, NULL, 0);

        char *bestmovePtr=strstr(response, "bestmove");
        if(bestmovePtr){
            sscanf(bestmovePtr, "bestmove %s", bestMove);
        }else{
            bestMove[0]='\0';
        }
    }

}



static void getFallbackMove(char *bestMove, ChessPiece board[8][8], int activeColor){
    Move allMoves[256];
    int sourceX[256];
    int sourceY[256];
    int count= 0;


    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(board[i][j].piece!=0&&board[i][j].isWhite==activeColor){
                ChessPiece tempPiece=board[i][j];
                calculatePossibleMoves(&tempPiece, board);
                filterIllegalMoves(&tempPiece, board);
                for(int m=0;m<tempPiece.numMoves;m++){
                    if(count<256){
                        allMoves[count]=tempPiece.possibleMoves[m];
                        sourceX[count]=i;
                        sourceY[count]= j;
                        count++;
                    }
                }
            }
        }
    }



    if(count>0){
        int idx=GetRandomValue(0, count-1);
        int srcCol=sourceY[idx];
        int srcRow=8-sourceX[idx];
        int destCol=allMoves[idx].y;
        int destRow=8-allMoves[idx].x;
        sprintf(bestMove, "%c%d%c%d", 'a'+srcCol, srcRow, 'a'+destCol, destRow);
    }else{
        bestMove[0]='\0';
    }
}





static void applyUCIMove(const char *moveStr, ChessPiece board[8][8]){
    if(strlen(moveStr)<4) return;

    int srcCol=moveStr[0]-'a';
    int srcRow= 8-(moveStr[1]-'0');
    int destCol=moveStr[2]-'a';
    int destRow=8-(moveStr[3]-'0');

    if(srcCol<0||srcCol>=8||srcRow<0||srcRow>=8||
        destCol<0||destCol>=8||destRow<0||destRow>=8){
        return;
    }



    ChessPiece dragged=board[srcRow][srcCol];
    dragged.x=destRow;
    dragged.y=destCol;
    dragged.hasMoved= 1;

    if(dragged.piece==6){
        if(dragged.isWhite==1){
            if(srcRow==7&&srcCol==4){
                if(destRow==7&&destCol==6){
                    board[7][5]= board[7][7];
                    board[7][5].x=7;
                    board[7][5].y=5;
                    board[7][5].hasMoved=1;
                    board[7][7]=createPiece(7, 7, 0, 0, -1, board[7][7]);
                }else if(destRow==7&&destCol==2){
                    board[7][3]=board[7][0];
                    board[7][3].x= 7;
                    board[7][3].y=3;
                    board[7][3].hasMoved=1;
                    board[7][0]=createPiece(7, 0, 0, 0, -1, board[7][0]);
                }
            }
        }else{
            if(srcRow==0&&srcCol==4){
                if(destRow==0&&destCol==6){
                    board[0][5]=board[0][7];
                    board[0][5].x=0;
                    board[0][5].y=5;
                    board[0][5].hasMoved= 1;
                    board[0][7]=createPiece(0, 7, 0, 0, -1, board[0][7]);
                }else if(destRow==0&&destCol==2){
                    board[0][3]=board[0][0];
                    board[0][3].x=0;
                    board[0][3].y= 3;
                    board[0][3].hasMoved=1;
                    board[0][0]=createPiece(0, 0, 0, 0, -1, board[0][0]);
                }
            }
        }
    }




    if(dragged.piece==1&&(destRow==0||destRow==7)){
        if(strlen(moveStr)>=5){
            char promo=moveStr[4];
            switch(promo){
                case 'q': dragged.piece=5; break;
                case 'r': dragged.piece=4; break;
                case 'b': dragged.piece=3; break;
                case 'n': dragged.piece=2; break;
                default: dragged.piece=5; break;
                       
            }
        }else{
            dragged.piece= 5;
        }
    }

    board[srcRow][srcCol]=createPiece(srcRow, srcCol, 0, 0, -1, board[srcRow][srcCol]);
    board[destRow][destCol]=dragged;
}





void StartSinglePlayer(){

    int pieceContor = 1;


    StartWindowTransition(1);
    isWhiteTurn = 1;
    gameOver = false;
    winnerColor = -1;
    gameOverTimer = 0.0f;

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


    chessboard[0][3] = createPiece(0, 3, pieceContor++, 5, 0, chessboard[0][3]);
    chessboard[7][3] = createPiece(7, 3, pieceContor++, 5, 1, chessboard[7][3]);


    chessboard[0][4] = createPiece(0, 4, pieceContor++, 6, 0, chessboard[0][4]);
    chessboard[7][4] = createPiece(7, 4, pieceContor++, 6, 1, chessboard[7][4]);





    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            printf("%d ",chessboard[i][j].piece);
        }
        printf("\n");
    }
}




void StartSinglePlayerAction(){
    CurrentWindow = 5;
}

void StartMultiplayer(){
    StartWindowTransition(2);
}

void CreateGameAction(){
    theSocketForTheServer = socket(2, 1, 0);
    struct sockaddr_in addr;
    addr.sin_family = 2;
    addr.sin_addr.s_addr = 0;
    int p = rand() % 50000 + 10000;
    addr.sin_port = htons(p);
    bind(theSocketForTheServer, (struct sockaddr *)&addr, sizeof(addr));
    theLobbyCodeNumber = p;
    fcntl(theSocketForTheServer, F_SETFL, O_NONBLOCK);
    listen(theSocketForTheServer, 1);
    
    isTheHostOfGame = true;
    isTheConnectionEstablishedNow = false;
    isTheGameInMultiplayerMode = true;
    CurrentWindow = 3;
}

void JoinGameAction(){
    stringForLobbyCodeInput[0] = '\0';
    lengthOfLobbyCodeInput = 0;
    didTheConnectionFail = false;
    CurrentWindow = 4;
}

void GoBackToMenu(){
    if(theSocketForTheClient >= 0){
        close(theSocketForTheClient);
        theSocketForTheClient = -1;
    }
    if(theSocketForTheServer >= 0){
        close(theSocketForTheServer);
        theSocketForTheServer = -1;
    }
    isTheConnectionEstablishedNow = false;
    isTheGameInMultiplayerMode = false;
    StartWindowTransition(0);
}



void JoinLobbyConnectAction(){
    if(lengthOfLobbyCodeInput == 0) return;
    int join_port = atoi(stringForLobbyCodeInput);
    
    theSocketForTheClient = socket(2, 1, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = 2;
    serv_addr.sin_port = htons(join_port);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    int res = connect(theSocketForTheClient, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if(res == 0){
        fcntl(theSocketForTheClient, F_SETFL, O_NONBLOCK);
        isTheGameInMultiplayerMode = true;
        isTheHostOfGame = false;
        playerColorNumberWhiteIs1BlackIs0 = 0;
        isTheConnectionEstablishedNow = true;
        didTheConnectionFail = false;
        StartSinglePlayer();
    }else{
        didTheConnectionFail = true;
        strcpy(stringForConnectionErrorMessage, "Failed to connect to lobby!");
        theSocketForTheClient = -1;
    }



}





int currentFrameIndex=0;


int main(){
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

    SingleplayerButton = createButton(WIDTH/2-150, HEIGHT/2-25+50, 50, 300, BLUE, DARKBLUE, &StartSinglePlayerAction, "Singleplayer");
    MultiplayerButton = createButton(WIDTH/2-150, HEIGHT/2-25+150, 50, 300, BLUE, DARKBLUE, &StartMultiplayer, "Multiplayer");
    CreateGameButton = createButton(WIDTH/2-150, HEIGHT/2-75, 50, 300, BLUE, DARKBLUE, &CreateGameAction, "Create Game");
    JoinGameButton = createButton(WIDTH/2-150, HEIGHT/2+25, 50, 300, BLUE, DARKBLUE, &JoinGameAction, "Join Game");
    LobbyBackButton = createButton(WIDTH/2-150, HEIGHT/2+125, 50, 300, BLUE, DARKBLUE, &GoBackToMenu, "Back");
    JoinConnectButton = createButton(WIDTH/2-150, HEIGHT/2+30, 50, 300, BLUE, DARKBLUE, &JoinLobbyConnectAction, "Connect");















    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);

        Vector2 mouse_pos = GetMousePosition();

        if(CurrentWindow == -1){
            int renderWindow;
            if(TransitionFrameCounter < 90){
                renderWindow = TransitionSourceWindow;
            }else{
                renderWindow = TransitionTargetWindow;
            }



            if(renderWindow == 0){
                showMenu(WIDTH, HEIGHT, currentFrameIndex);
                drawButton(SingleplayerButton, mouse_pos, 20, BLACK);
                drawButton(MultiplayerButton, mouse_pos, 20, BLACK);
            } else if(renderWindow == 1){
                for(int i=0; i<8; i++){
                    for(int j=0; j<8; j++){
                        Color col = ((i + j) % 2 == 0) ? LIGHTGRAY : DARKGRAY;
                        DrawRectangle(i*100, j*100, 100, 100, col);
                    }
                }
            } else if(renderWindow == 2){
                showMenu(WIDTH, HEIGHT, currentFrameIndex);
                drawButton(CreateGameButton, mouse_pos, 20, BLACK);
                drawButton(JoinGameButton, mouse_pos, 20, BLACK);
                drawButton(LobbyBackButton, mouse_pos, 20, BLACK);
            }
            ShowTransitionAnimation(WIDTH, HEIGHT, TransitionFrameCounter);

            TransitionFrameCounter++;
            if(TransitionFrameCounter >= 180){
                CurrentWindow = TransitionTargetWindow;
                TransitionFrameCounter = 0;

                if(CurrentWindow == 0 || CurrentWindow == 2){
                    if(theSocketForTheClient >= 0){
                        close(theSocketForTheClient);
                        theSocketForTheClient = -1;
                    }


                    if(theSocketForTheServer >= 0){
                        close(theSocketForTheServer);
                        theSocketForTheServer = -1;
                    }
                    isTheConnectionEstablishedNow = false;
                    isTheGameInMultiplayerMode = false;
                }

            }
        } else if(CurrentWindow== 1){

            if(!gameOver){
                if(isKingInCheck(isWhiteTurn, chessboard)&&!hasAnyLegalMoves(isWhiteTurn, chessboard)){
                    gameOver= true;
                    winnerColor=1-isWhiteTurn;
                    gameOverTimer=0.0f;
                }

                bool shouldWeReceiveAMove = false;
                if(isTheGameInMultiplayerMode == true){
                    if(isTheConnectionEstablishedNow == true){
                        if(isWhiteTurn != playerColorNumberWhiteIs1BlackIs0){
                            shouldWeReceiveAMove = true;
                        }
                    }
                }

                if(shouldWeReceiveAMove == true){
                    char buf[100];
                    int n = recv(theSocketForTheClient, buf, 99, 0);
                    if(n > 0){
                        buf[n] = '\0';
                        applyUCIMove(buf, chessboard);
                        isWhiteTurn = 1 - isWhiteTurn;
                    }else if(n == 0){
                        close(theSocketForTheClient);
                        theSocketForTheClient = -1;
                        isTheConnectionEstablishedNow = false;
                        isTheGameInMultiplayerMode = false;
                        StartWindowTransition(0);
                    }
                }

                if(!gameOver&&isWhiteTurn==0&&!isTheGameInMultiplayerMode){
                    char fen[256];
                    generateFEN(fen, chessboard, 0);

                    char bestMove[16]={0};
                    int skill = 0;
                    if(stockfishDifficultyLevelNumber == 1) skill = 0;
                    if(stockfishDifficultyLevelNumber == 2) skill = 2;
                    if(stockfishDifficultyLevelNumber == 3) skill = 4;
                    if(stockfishDifficultyLevelNumber == 4) skill = 6;
                    if(stockfishDifficultyLevelNumber == 5) skill = 8;
                    if(stockfishDifficultyLevelNumber == 6) skill = 10;
                    if(stockfishDifficultyLevelNumber == 7) skill = 12;
                    if(stockfishDifficultyLevelNumber == 8) skill = 14;
                    if(stockfishDifficultyLevelNumber == 9) skill = 16;
                    if(stockfishDifficultyLevelNumber == 10) skill = 20;
                    getStockfishMove(bestMove, fen, skill);

                    if(strlen(bestMove)==0){
                        getFallbackMove(bestMove, chessboard, 0);
                    }

                    if(strlen(bestMove)>0){
                        applyUCIMove(bestMove, chessboard);
                    }

                    isWhiteTurn=1;
                }
            }else{
                gameOverTimer+=GetFrameTime();
                if(gameOverTimer>=3.0f){
                    gameOver= false;
                    StartWindowTransition(0);
                }
            }

            for(int i=0; i<8; i++){
                for(int j=0; j<8; j++){
                    Color col = ((i + j) % 2 == 0) ? LIGHTGRAY : DARKGRAY;
                    DrawRectangle(i*100, j*100, 100, 100, col);
                }
            }

            if(isKingInCheck(isWhiteTurn, chessboard)){
                int kx=-1, ky=-1;
                findKing(isWhiteTurn, chessboard, &kx, &ky);
                if(kx!=-1&&ky!=-1){
                    DrawRectangle(ky*100, kx*100, 100, 100, (Color){255, 0, 0, 120});
                    DrawText("CHECK!", ky*100+10, kx*100+35, 20, RAYWHITE);
                }
            }
            int mouseCellX = (int)(mouse_pos.x / 100);
            int mouseCellY = (int)(mouse_pos.y / 100);
            if(mouseCellX < 0) mouseCellX = 0;
            if(mouseCellX > 7) mouseCellX = 7;
            if(mouseCellY < 0) mouseCellY = 0;
            if(mouseCellY > 7) mouseCellY = 7;

            if(!gameOver&&IsMouseButtonPressed(MOUSE_BUTTON_LEFT)&&!dragging){
                if(chessboard[mouseCellY][mouseCellX].piece!=0&&chessboard[mouseCellY][mouseCellX].isWhite==isWhiteTurn&&(!isTheGameInMultiplayerMode||playerColorNumberWhiteIs1BlackIs0==isWhiteTurn)){
                    dragging= true;
                    draggedPiece= chessboard[mouseCellY][mouseCellX];
                    dragSourceX= mouseCellX;
                    dragSourceY= mouseCellY;
                    selectedPieceX= mouseCellX;
                    selectedPieceY= mouseCellY;
                    calculatePossibleMoves(&draggedPiece, chessboard);
                    if(draggedPiece.piece==6){
                        calculateCastlingMoves(&draggedPiece, chessboard);
                    }
                    filterIllegalMoves(&draggedPiece, chessboard);
                    chessboard[dragSourceY][dragSourceX].piece= 0;
                    chessboard[dragSourceY][dragSourceX].isWhite=0;
                }

            }
            if(dragging&&IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
                int dropX=(int)(mouse_pos.x/100);
                int dropY=(int)(mouse_pos.y/100);
                if(dropX<0) dropX=0;
                if(dropX>7) dropX=7;
                if(dropY<0) dropY=0;
                if(dropY>7) dropY=7;

                bool isValidMove=false;
                for(int m=0;m<draggedPiece.numMoves;m++){
                    if(draggedPiece.possibleMoves[m].x==dropY&&
                       draggedPiece.possibleMoves[m].y==dropX){
                        isValidMove=true;
                        break;
                    }
                }


                if(isValidMove){
                    draggedPiece.x=dropY;
                    draggedPiece.y=dropX;
                    draggedPiece.hasMoved= 1;

                    if(draggedPiece.piece==6){
                        if(draggedPiece.isWhite==1){
                            if(dragSourceY==7&&dragSourceX==4){
                                if(dropY==7&&dropX==6){
                                    chessboard[7][5]=chessboard[7][7];
                                    chessboard[7][5].x= 7;
                                    chessboard[7][5].y=5;
                                    chessboard[7][5].hasMoved=1;
                                    chessboard[7][7]=createPiece(7, 7, 0, 0, -1, chessboard[7][7]);
                                }else if(dropY==7&&dropX==2){
                                    chessboard[7][3]=chessboard[7][0];
                                    chessboard[7][3].x=7;
                                    chessboard[7][3].y= 3;
                                    chessboard[7][3].hasMoved=1;
                                    chessboard[7][0]=createPiece(7, 0, 0, 0, -1, chessboard[7][0]);
                                }
                            }
                        }else{
                            if(dragSourceY==0&&dragSourceX==4){
                                if(dropY==0&&dropX==6){
                                    chessboard[0][5]= chessboard[0][7];
                                    chessboard[0][5].x=0;
                                    chessboard[0][5].y=5;
                                    chessboard[0][5].hasMoved= 1;
                                    chessboard[0][7]=createPiece(0, 7, 0, 0, -1, chessboard[0][7]);
                                }else if(dropY==0&&dropX==2){
                                    chessboard[0][3]= chessboard[0][0];
                                    chessboard[0][3].x=0;

                                    chessboard[0][3].y=3;
                                    chessboard[0][3].hasMoved=1;
                                    chessboard[0][0]=createPiece(0, 0, 0, 0, -1, chessboard[0][0]);
                                }
                            }


                        }
                    }

                    if(isTheGameInMultiplayerMode && isTheConnectionEstablishedNow){
                        char moveStr[16];
                        sprintf(moveStr, "%c%d%c%d", 'a' + dragSourceX, 8 - dragSourceY, 'a' + dropX, 8 - dropY);
                        send(theSocketForTheClient, moveStr, 5, 0);
                    }
                    chessboard[dropY][dropX]=draggedPiece;
                    isWhiteTurn= 1-isWhiteTurn;
                }else{
                    chessboard[dragSourceY][dragSourceX]= draggedPiece;
                }

                dragging=false;
                dragSourceX=-1;
                dragSourceY=-1;
                selectedPieceX=-1;
                selectedPieceY=-1;
            }

            for(int i=0;i<8;i++){
                for(int j=0;j<8;j++){

                    if(chessboard[i][j].piece==0) continue;

                    Texture2D tex = getPieceTexture(&chessboard[i][j]);
                    if(tex.id != 0){
                        Rectangle src = {0, 0, (float)tex.width, (float)tex.height};
                        Rectangle dest = {(float)j*100.0f, (float)i*100.0f, 100.0f, 100.0f};
                        Vector2 origin = {0, 0};
                        DrawTexturePro(tex, src, dest, origin, 0.0f, WHITE);
                    }
                }
            }

            if(selectedPieceX != -1 && selectedPieceY != -1){
                for(int m = 0; m < draggedPiece.numMoves; m++){
                    int moveX = draggedPiece.possibleMoves[m].x; 
                    int moveY = draggedPiece.possibleMoves[m].y; 
                    float centerX = moveY * 100.0f + 50.0f;
                    float centerY = moveX * 100.0f + 50.0f;


                    if(chessboard[moveX][moveY].piece != 0){
                        DrawCircleLines((int)centerX, (int)centerY, 45, RED);
                        DrawCircleLines((int)centerX, (int)centerY, 44, RED);
                        DrawCircleLines((int)centerX, (int)centerY, 43, RED);
                    } else {
                        DrawCircle((int)centerX, (int)centerY, 15, LIME);
                    }
                }
            }

            if(dragging){
                Texture2D tex = getPieceTexture(&draggedPiece);
                if(tex.id != 0){
                    Rectangle src = {0, 0, (float)tex.width, (float)tex.height};
                    Rectangle dest = {mouse_pos.x - 50.0f, mouse_pos.y - 50.0f, 100.0f, 100.0f};
                    Vector2 origin = {0, 0};
                    DrawTexturePro(tex, src, dest, origin, 0.0f, WHITE);
                }
            }
            if(gameOver){
                DrawRectangle(0, 0, WIDTH, HEIGHT, (Color){0, 0, 0, 180});
                char stringForWinnerColor[10];
                if(winnerColor == 1){
                    strcpy(stringForWinnerColor, "WHITE");
                }else{
                    strcpy(stringForWinnerColor, "BLACK");
                }
                char winText[50];
                sprintf(winText, "%s WINS!", stringForWinnerColor);
                int winFontSize=45;
                int textWidth=MeasureText(winText, winFontSize);
                DrawText(winText, WIDTH/2-textWidth/2, HEIGHT/2-45, winFontSize, GOLD);

                char subText[]="Checkmate! Returning to main menu...";
                int subFontSize=22;
                int subWidth=MeasureText(subText, subFontSize);
                DrawText(subText, WIDTH/2-subWidth/2, HEIGHT/2+25, subFontSize, RAYWHITE);
            }

        } else if(CurrentWindow == 0){
            showMenu(WIDTH, HEIGHT, currentFrameIndex);
            char title[] = "CCC";
            int titleSize = 60;

            int titleW = MeasureText(title, titleSize);
            DrawText(title, WIDTH/2 - titleW/2, HEIGHT/2 - 120, titleSize, BLACK);
            drawButton(SingleplayerButton, mouse_pos, 20, BLACK);
            drawButton(MultiplayerButton, mouse_pos, 20, BLACK);
        }else if(CurrentWindow==2){
            showMenu(WIDTH, HEIGHT, currentFrameIndex);
            drawButton(CreateGameButton, mouse_pos, 20, BLACK);
            drawButton(JoinGameButton, mouse_pos, 20, BLACK);
            drawButton(LobbyBackButton, mouse_pos, 20, BLACK);
        }else if(CurrentWindow == 3){
            showMenu(WIDTH, HEIGHT, currentFrameIndex);
            char lobbyMsg[64];
            sprintf(lobbyMsg, "LOBBY CODE: %d", theLobbyCodeNumber);
            int lobbyMsgSize = 32;
            int textW = MeasureText(lobbyMsg, lobbyMsgSize);
            DrawText(lobbyMsg, WIDTH/2 - textW/2, HEIGHT/2 - 50, lobbyMsgSize, BLACK);
            drawButton(LobbyBackButton, mouse_pos, 20, BLACK);
            int client_fd = accept(theSocketForTheServer, NULL, NULL);
            if(client_fd >= 0){
                theSocketForTheClient = client_fd;
                fcntl(theSocketForTheClient, F_SETFL, O_NONBLOCK);
                isTheGameInMultiplayerMode = true;
                isTheHostOfGame = true;
                playerColorNumberWhiteIs1BlackIs0 = 1;
                isTheConnectionEstablishedNow = true;
                StartSinglePlayer();
            }


        }else if(CurrentWindow == 4){
            showMenu(WIDTH, HEIGHT, currentFrameIndex);
            char promptMsg[] = "ENTER LOBBY CODE:";
            int promptMsgSize = 24;
            int promptW = MeasureText(promptMsg, promptMsgSize);
            DrawText(promptMsg, WIDTH/2 - promptW/2, HEIGHT/2 - 80, promptMsgSize, GOLD);
            DrawRectangle(WIDTH/2 - 150, HEIGHT/2 - 30, 300, 50, LIGHTGRAY);
            DrawRectangleLines(WIDTH/2 - 150, HEIGHT/2 - 30, 300, 50, DARKGRAY);
            int textW = MeasureText(stringForLobbyCodeInput, 24);
            DrawText(stringForLobbyCodeInput, WIDTH/2 - textW/2, HEIGHT/2 - 18, 24, BLACK);
            int key = GetCharPressed();
            while(key > 0){
                if(key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9'){
                    if(lengthOfLobbyCodeInput < 10){
                        stringForLobbyCodeInput[lengthOfLobbyCodeInput] = key;
                        lengthOfLobbyCodeInput = lengthOfLobbyCodeInput + 1;
                        stringForLobbyCodeInput[lengthOfLobbyCodeInput] = '\0';
                    }
                }
                key = GetCharPressed();
            }

            if(IsKeyPressed(KEY_BACKSPACE)){
                if(lengthOfLobbyCodeInput > 0){
                    lengthOfLobbyCodeInput--;
                    stringForLobbyCodeInput[lengthOfLobbyCodeInput] = '\0';
                }
            }
            drawButton(JoinConnectButton, mouse_pos, 20, BLACK);
            drawButton(LobbyBackButton, mouse_pos, 20, BLACK);
            if(didTheConnectionFail){
                int errW = MeasureText(stringForConnectionErrorMessage, 18);
                DrawText(stringForConnectionErrorMessage, WIDTH/2 - errW/2, HEIGHT/2 + 90, 18, RED);
            }
        }else if(CurrentWindow == 5){
            showMenu(WIDTH, HEIGHT, currentFrameIndex);
            char diffMsg[] = "SELECT DIFFICULTY";
            int diffMsgSize = 32;
            int textW = MeasureText(diffMsg, diffMsgSize);
            DrawText(diffMsg, WIDTH/2 - textW/2, HEIGHT/2 - 150, diffMsgSize, GOLD);
            int bx = WIDTH/2 - 180;
            int by = HEIGHT/2 - 50;
            for(int i = 1; i <= 10; i++){
                bool hovered = false;
                if(mouse_pos.x >= bx){
                    if(mouse_pos.x <= bx + 60){
                        if(mouse_pos.y >= by){
                            if(mouse_pos.y <= by+ 60){
                                hovered = true;
                            }
                        }

                    }


                }
                if(hovered){
                    DrawRectangle(bx, by, 60, 60, DARKBLUE);
                }else{
                    DrawRectangle(bx, by, 60, 60, BLUE);
                }
                char numStr[10];
                sprintf(numStr, "%d", i);
                DrawText(numStr, bx + 20, by + 20, 24, BLACK);
                if(hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                    stockfishDifficultyLevelNumber = i;
                    isTheGameInMultiplayerMode = false;
                    StartSinglePlayer();
                }
                bx = bx + 75;
                if(i == 5){
                    bx = WIDTH/2 - 180;
                    by = by + 75;
                }
            }
            drawButton(LobbyBackButton, mouse_pos, 20, BLACK);
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

    if(theSocketForTheClient >= 0) close(theSocketForTheClient);
    if(theSocketForTheServer >= 0) close(theSocketForTheServer);
    CloseWindow();
    return 0;
}