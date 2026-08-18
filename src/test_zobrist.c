#include <stdio.h>

#include "board.h"
#include "move.h"
#include "zobrist.h"

void TestZobrist(void)
{
    Position position;

    InitBoard(&position);

    MoveList moveList;

    GenerateLegalMoves(&position, &moveList);

    printf("=== TEST ZOBRIST ===\n");

    for (int i = 0; i < moveList.count; i++)
    {
        Move move = moveList.moves[i];

        uint64_t originalHash = position.hash;

        UndoInfo undo =
            MakeMoveWithUndo(&position, move);

        uint64_t recomputedHash =
            ComputeZobristHash(&position);

        if (position.hash != recomputedHash)
        {
            printf("FAIL : HASH APRES MAKE MOVE\n");
            printf("Coup : %d -> %d\n",
                   move.fromRow * 8 + move.fromCol,
                   move.toRow * 8 + move.toCol);
            return;
        }

        UndoMove(&position, move, undo);

        recomputedHash =
            ComputeZobristHash(&position);

        if (position.hash != recomputedHash)
        {
            printf("FAIL : HASH APRES UNDO\n");
            return;
        }

        if (position.hash != originalHash)
        {
            printf("FAIL : HASH ORIGINAL NON RESTAURE\n");
            return;
        }
    }

    printf("PASS : TOUS LES HASHES SONT CORRECTS\n");
}