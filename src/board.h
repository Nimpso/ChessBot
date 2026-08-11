#ifndef BOARD_H
#define BOARD_H

typedef struct
{
    char board[8][8];
    int sideToMove;
} Position;

void InitBoard(Position *position);
void PrintBoard(Position *position);
int IsWhitePiece(char piece);
int IsBlackPiece(char piece);

#endif