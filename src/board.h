#ifndef BOARD_H
#define BOARD_H

typedef struct
{
    char board[8][8];
    int sideToMove;

    int whiteKingSideCastle;
    int whiteQueenSideCastle;
    int blackKingSideCastle;
    int blackQueenSideCastle;

    
    //-1 si aucune prise en passant n'est possible ce coup-ci.
    //Sinon, c'est la case "sautée" par un pion qui vient
    //d'avancer de 2 cases (la case que le pion adverse peut viser pour capturer en passant)
    
    int enPassantRow;
    int enPassantCol;
    int halfMoveClock;
} Position;

void InitBoard(Position *position);
void PrintBoard(Position *position);
int IsWhitePiece(char piece);
int IsBlackPiece(char piece);

#endif