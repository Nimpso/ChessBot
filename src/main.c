#include <stdio.h>
#include <time.h>

#include "board.h"
#include "move.h"
#include "eval.h"
#include "search.h"


void PrintMove(Move move)
{
    printf("%c%d -> %c%d",
           'a' + move.fromCol, 8 - move.fromRow,
           'a' + move.toCol,   8 - move.toRow);
 
    if (move.promotion != '\0')
    {
        printf("=%c", move.promotion);
    }
}

void TestSelfPlayIterativeDeepening(int maxHalfMoves, int maxDepth, double timePerMove)
{
    Position position;
    InitBoard(&position);

    printf("\n");
    printf("========================================\n");
    printf("   TEST 8 : SELF-PLAY (iterative deepening, %.1fs/coup)\n", timePerMove);
    printf("========================================\n\n");

    PrintBoard(&position);

    for (int ply = 1; ply <= maxHalfMoves; ply++)
    {
        if (IsFiftyMoveRule(&position))
        {
            printf("\n>>> NULLE (regle des 50 coups sans capture ni coup de pion).\n");
            return;
        }

        MoveList legalMoves;
        GenerateLegalMoves(&position, &legalMoves);

        if (legalMoves.count == 0)
        {
            if (IsCheckmate(&position))
            {
                printf("\n>>> ECHEC ET MAT. Les %s gagnent.\n",
                       position.sideToMove == 0 ? "noirs" : "blancs");
            }
            else
            {
                printf("\n>>> PAT. Partie nulle.\n");
            }
            return;
        }

        clock_t start = clock();
        SearchResult result = IterativeDeepening(&position, maxDepth, timePerMove);
        Move best = result.move;  // Extraire le Move de la structure SearchResult
        double elapsed = (double)(clock() - start) / CLOCKS_PER_SEC;

        printf("%3d. %s joue : %c%d -> %c%d   (score : %d, profondeur: %d, %.2fs)\n",
        ply,
        position.sideToMove == 0 ? "Blancs" : "Noirs ",
        'a' + best.fromCol, 8 - best.fromRow,
        'a' + best.toCol,   8 - best.toRow,
        result.score,
        result.depth,
        elapsed);

        if (best.promotion != '\0')
        {
            printf("=%c", best.promotion);
        }

        MakeMove(&position, best);

        printf("   (score : %d, %.2fs)\n", Evaluate(&position), elapsed);
        if(ply % 5 == 0)
        {
            PrintBoard(&position);
        }
    }

    printf("\n>>> Limite de %d demi-coups atteinte, partie non terminee.\n",
           maxHalfMoves);
    printf("\n");
    PrintBoard(&position);
}

void TestPositionFromFEN(const char *fen, const char *description, double thinkTimeSeconds, int maxDepth)
{
    Position position;
    InitPositionFromFEN(&position, fen);

    printf("\n");
    printf("========================================\n");
    printf("   TEST POSITION : %s\n", description);
    printf("========================================\n\n");

    printf("FEN : %s\n\n", fen);
    PrintBoard(&position);
    printf("\nTrait aux %s\n", position.sideToMove == 0 ? "blancs" : "noirs");
    printf("Temps de reflexion : %.1f sec\n\n", thinkTimeSeconds);

    SearchResult result = IterativeDeepening(&position, maxDepth, thinkTimeSeconds);

    printf("Meilleur coup trouve : ");
    PrintMove(result.move);
    printf("\n");
    printf("Evaluation : %d\n", result.score);
    printf("Profondeur atteinte : %d\n", result.depth);
    printf("\n");
}


/* ============================================================
   MAIN
   ============================================================ */

int main(void)
{
    printf("\n");
    printf("========================================\n");
    printf("          CHESSBOT - TESTS\n");
    printf("========================================\n");

    Position position;
    InitBoard(&position);
 
 
    double thinkTimeSeconds = 5.0;
    int maxDepth = 30; 
 
 
    /*printf("Position analysee :\n\n");
    PrintBoard(&position);
    printf("\nTrait aux %s\n", position.sideToMove == 0 ? "blancs" : "noirs");
    printf("Temps de reflexion : %.1f sec\n\n", thinkTimeSeconds);
 
    SearchResult result = IterativeDeepening(&position, maxDepth, thinkTimeSeconds);
 
    printf("Meilleur coup trouve : ");
    PrintMove(result.move);
    printf("\n");
    printf("Evaluation : %d\n", result.score);
    printf("Profondeur atteinte : %d\n", result.depth);*/

    //TestSelfPlayIterativeDeepening(500, 10, 2);

    // Test 1 : MAT EN 1
    TestPositionFromFEN(
        "7k/6Q1/6K1/8/8/8/8/8 w - - 0 1",
        "Mat en 1 ",
        thinkTimeSeconds,
        maxDepth
    );

    // Test 2 : finale
    TestPositionFromFEN(
        "6k1/5ppp/8/8/8/8/5Q2/6K1 w - - 0 1",
        "Finale dame - recherche de mat",
        thinkTimeSeconds,
        maxDepth
    );

    // Test 3 : prise
    TestPositionFromFEN(
        "r3k2r/ppp2ppp/2n5/3q4/8/4N3/PPPP1PPP/R2QK2R w KQkq - 0 1",
        "gain de matériel",
        thinkTimeSeconds,
        maxDepth
    );

    // Test 4 : TACTIQUE DE GAIN DE PIECE
    TestPositionFromFEN(
        "r1bqk2r/pppp1ppp/2n2n2/8/2B1P3/2N2N2/PPPP1PPP/R1BQK2R w KQkq - 0 1",
        "Milieu de jeu - tactique et développement",
        thinkTimeSeconds,
        maxDepth
    );

    // Test 5 : finale nulle
    TestPositionFromFEN(
        "k7/P7/8/8/8/8/8/6K1 w - - 0 1",
        "finale nulle",
        thinkTimeSeconds,
        maxDepth
    );

    // Test 6 : PRISE EN PASSANT
    TestPositionFromFEN(
        "4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1",
        "Prise en passant",
        thinkTimeSeconds,
        maxDepth
    );

    // Test 7 : ROQUE
    TestPositionFromFEN(
        "r3k2r/pppq1ppp/2npbn2/8/2B1P3/2N1BN2/PPP2PPP/R2QK2R w KQkq - 0 1",
        "Milieu de jeu - roque et développement roque a ne pas faire",
        thinkTimeSeconds,
        maxDepth
    );

    // Test 8 : MILIEU DE JEU COMPLEXE
    TestPositionFromFEN(
        "r1bq1rk1/ppp2ppp/2np1n2/8/2B1P3/2N1BN2/PPP2PPP/R2Q1RK1 w - - 0 1",
        "Milieu de jeu complexe - comparaison Stockfish",
        thinkTimeSeconds,
        maxDepth
    );


    printf("\n");
    printf("========================================\n");
    printf("             TESTS TERMINES\n");
    printf("========================================\n\n");

    return 0;
}