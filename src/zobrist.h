#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <stdint.h>
#include "board.h"

void InitZobrist(void);
uint64_t ComputeZobristHash(Position *position);

uint64_t GetPieceHash(char piece, int row, int col);
uint64_t GetSideHash(void);
uint64_t GetCastleHash(int castleIndex);
uint64_t GetEnPassantHash(int col);

void TestZobrist(void);

#endif