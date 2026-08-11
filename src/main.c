#include <stdio.h>
#include "board.h"
#include "move.h"

void PrintMoveList(MoveList *moveList)
{
    printf("\nNombre de coups : %d\n", moveList->count);

    for (int i = 0; i < moveList->count; i++)
    {
        Move move = moveList->moves[i];

        printf("%2d. %c%d -> %c%d\n",
               i + 1,
               'a' + move.fromCol,
               8 - move.fromRow,
               'a' + move.toCol,
               8 - move.toRow);
    }
}

void PrintSide(Position *position)
{
    printf("\nSide : %s\n",
           position->sideToMove == 0 ? "White" : "Black");
}

int main(void)
{
    Position position;
    MoveList moveList;

    printf("========== CHESSBOT TEST ==========\n");

    /*
     * TEST 1 : position initiale
     */
    printf("\n===== TEST 1 : POSITION INITIALE =====\n");

    InitBoard(&position);
    PrintBoard(&position);
    PrintSide(&position);

    GeneratePawnMoves(&position, &moveList);
    PrintMoveList(&moveList);


    /*
     * TEST 2 : e2-e4
     */
    printf("\n===== TEST 2 : e2-e4 =====\n");

    Move move;

    move.fromRow = 6;
    move.fromCol = 4;
    move.toRow = 4;
    move.toCol = 4;

    MakeMove(&position, move);

    PrintBoard(&position);
    PrintSide(&position);

    GeneratePawnMoves(&position, &moveList);
    PrintMoveList(&moveList);


    /*
     * TEST 3 : e7-e5
     */
    printf("\n===== TEST 3 : e7-e5 =====\n");

    move.fromRow = 1;
    move.fromCol = 4;
    move.toRow = 3;
    move.toCol = 4;

    MakeMove(&position, move);

    PrintBoard(&position);
    PrintSide(&position);

    GeneratePawnMoves(&position, &moveList);
    PrintMoveList(&moveList);


    return 0;
}