#include <stdio.h>
#include "move.h"

void MakeMove(Position *position, Move move)
{
    position->board[move.toRow][move.toCol] = position->board[move.fromRow][move.fromCol]; //case maintenant = case avant
    position->board[move.fromRow][move.fromCol] = '.'; //case avant = '.'
    position->sideToMove == 0 ? position->sideToMove = 1 : 0; //side change

    printf("%c%d -> %c%d\n",
    'a' + move.fromCol,
    8 - move.fromRow,
    'a' + move.toCol,
    8 - move.toRow);
}