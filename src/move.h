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

void MakeMove(Position *position, Move move);

#endif