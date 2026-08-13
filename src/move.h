#ifndef MOVE_H
#define MOVE_H

#include "board.h"

typedef struct
{
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;

//'\0' si ce n'est pas une promotion Sinon : 'Q', 'R', 'B', 'N' (blancs) ou 'q', 'r', 'b', 'n' (noirs).
//On stocke directement le caractère avec la bonne casse
    char promotion;

    //1 si ce en passant, 0 sinon
    int enPassant;
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
void CopyPosition(Position *source, Position *destination);
void AddMove(MoveList *moveList, int fromRow, int fromCol, int toRow, int toCol);
void AddPromotionMove(MoveList *moveList, int fromRow, int fromCol, int toRow, int toCol, char promotion);
void AddEnPassantMove(MoveList *moveList, int fromRow, int fromCol, int toRow, int toCol);
void GenerateCastlingMoves(Position *position, MoveList *moveList);
void GenerateLegalMoves(Position *position, MoveList *legalMoves);
void GeneratePseudoLegalMoves(Position *position, MoveList *moveList);
long long Perft(Position *position, int depth);
int IsCheckmate(Position *position);
int IsStalemate(Position *position);
int IsFiftyMoveRule(Position *position);

#endif