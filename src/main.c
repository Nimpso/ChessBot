#include <stdio.h>
#include "board.h"

int main(void)
{
    Position myposition;

    printf("ChessBot\n");

    InitBoard(&myposition);
    PrintBoard(&myposition);

    return 0;
}  