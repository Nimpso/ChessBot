#include <stdio.h>
#include "board.h"
#include "move.h"

int main(void)
{
    Position myposition;

    printf("ChessBot\n");

    InitBoard(&myposition);
    PrintBoard(&myposition);

    printf("Side : %s\n",myposition.sideToMove == 0 ? "White" : "Black");

    Move move_test;

    move_test.fromRow = 6;
    move_test.fromCol = 4;
    move_test.toRow = 4;
    move_test.toCol = 4;

    MakeMove(&myposition, move_test);
    PrintBoard(&myposition);

    printf("Side : %s\n",myposition.sideToMove == 0 ? "White" : "Black");

    move_test.fromRow = 1;
    move_test.fromCol = 4;
    move_test.toRow = 3;
    move_test.toCol = 4;

    MakeMove(&myposition, move_test);
    PrintBoard(&myposition);

    printf("Side : %s\n",myposition.sideToMove == 0 ? "White" : "Black");


    return 0;
}  