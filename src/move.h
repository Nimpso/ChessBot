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

#endif