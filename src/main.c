#include <stdio.h>
#include "board.h"
#include "move.h"
#include "board.h"

int main(void)
{
    Position myposition;

    printf("ChessBot\n");

    InitBoard(&myposition);
    PrintBoard(&myposition);

    Move move_test;

    move_test.fromRow = 6;
    move_test.fromCol = 4;
    move_test.toRow = 4;
    move_test.toCol = 4;

    MakeMove(&myposition, move_test);
    PrintBoard(&myposition);


    return 0;
}  