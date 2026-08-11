#ifndef MOVE_H
#define MOVE_H

#include "board.h"

typedef struct
{
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;
} Move;

typedef struct
{
    Move moves[256];
    int count;
} MoveList;

void MakeMove(Position *position, Move move);
void GeneratePawnMoves(Position *position, MoveList *moveList);
void GenerateKnightMoves(Position *position, MoveList *moveList);
void GenerateBishopMoves(Position *position, MoveList *moveList);
void GenerateRookMoves(Position *position, MoveList *moveList);
void GenerateQueenMoves(Position *position, MoveList *moveList);
void GenerateKingMoves(Position *position, MoveList *moveList);
int IsSquareAttacked(Position *position, int row, int col, int bySide);
int IsInCheck(Position *position, int side);
void AddMove(MoveList *moveList, int fromRow, int fromCol, int toRow, int toCol);
void GenerateLegalMoves(Position *position, MoveList *legalMoves);
void GeneratePseudoLegalMoves(Position *position, MoveList *moveList);


#endif