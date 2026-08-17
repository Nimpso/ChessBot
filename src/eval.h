#ifndef EVAL_H
#define EVAL_H

#include "board.h"

//1 pion d'avance = 100

int Evaluate(Position *position);

int PieceValue(char piece);
int PieceSquareValue(char piece, int row, int col);
int GamePhase(Position *position);

#endif