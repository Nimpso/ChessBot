#include <stdio.h>
#include <time.h>
#include <string.h>

#include "board.h"
#include "move.h"
#include "eval.h"
#include "search.h"
#include "zobrist.h"
#include "tt.h"
#include "uci.h"


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


/* ============================================================
   TEST SELF-PLAY
   ============================================================ */

void TestSelfPlayIterativeDeepening(
    int maxHalfMoves,
    int maxDepth,
    double timePerMove)
{
    Position position;

    InitBoard(&position);

    printf("\n");
    printf("========================================\n");
    printf("   TEST 8 : SELF-PLAY\n");
    printf("   Iterative Deepening + TT\n");
    printf("   %.1fs / coup\n", timePerMove);
    printf("========================================\n\n");

    PrintBoard(&position);

    for (int ply = 1; ply <= maxHalfMoves; ply++)
    {
        if (IsFiftyMoveRule(&position))
        {
            printf("\n>>> NULLE (regle des 50 coups).\n");
            return;
        }

        MoveList legalMoves;

        GenerateLegalMoves(
            &position,
            &legalMoves
        );

        if (legalMoves.count == 0)
        {
            if (IsCheckmate(&position))
            {
                printf(
                    "\n>>> ECHEC ET MAT. Les %s gagnent.\n",
                    position.sideToMove == 0
                        ? "noirs"
                        : "blancs"
                );
            }
            else
            {
                printf("\n>>> PAT. Partie nulle.\n");
            }

            return;
        }

        /*
         * Pour chaque coup de la partie :
         * nouvelle recherche = nouvelles statistiques.
         *
         * La TT est vidée ici uniquement parce que ce test
         * sert à mesurer chaque recherche indépendamment.
         */
        TT_Clear();
        TT_ResetStats();

        clock_t start = clock();

        SearchResult result =
            IterativeDeepening(
                &position,
                maxDepth,
                timePerMove,
                NULL
            );

        Move best = result.move;

        double elapsed =
            (double)(clock() - start)
            / CLOCKS_PER_SEC;

        printf("\n");
        printf("----------------------------------------\n");

        printf(
            "Coup %d - %s joue : ",
            ply,
            position.sideToMove == 0
                ? "Blancs"
                : "Noirs"
        );

        PrintMove(best);

        printf("\n");

        printf(
            "Score       : %d\n",
            result.score
        );

        printf(
            "Profondeur  : %d\n",
            result.depth
        );

        printf(
            "Temps       : %.2fs\n",
            elapsed
        );

        /*
         * Statistiques TT
         */
        TT_PrintStats();

        /*
         * On joue le meilleur coup
         */
        MakeMove(
            &position,
            best
        );

        printf(
            "Evaluation apres coup : %d\n",
            Evaluate(&position)
        );

        if (ply % 5 == 0)
        {
            printf("\n");
            PrintBoard(&position);
        }
    }

    printf(
        "\n>>> Limite de %d demi-coups atteinte.\n",
        maxHalfMoves
    );

    printf("\n");
    PrintBoard(&position);
}


/* ============================================================
   TEST D'UNE POSITION FEN
   ============================================================ */

void TestPositionFromFEN(
    const char *fen,
    const char *description,
    double thinkTimeSeconds,
    int maxDepth)
{
    Position position;

    InitPositionFromFEN(
        &position,
        fen
    );

    printf("\n");
    printf("========================================\n");
    printf("   TEST POSITION : %s\n", description);
    printf("========================================\n\n");

    printf("FEN : %s\n\n", fen);

    PrintBoard(&position);

    printf("\n");
    printf(
        "Trait aux %s\n",
        position.sideToMove == 0
            ? "blancs"
            : "noirs"
    );

    printf(
        "Temps de reflexion : %.1f sec\n",
        thinkTimeSeconds
    );

    /*
     * --------------------------------------------------------
     * TT PROPRE POUR CETTE POSITION
     * --------------------------------------------------------
     */

    TT_Clear();
    TT_ResetStats();

    /*
     * Vérification du hash avant recherche
     */

    uint64_t computedHash =
        ComputeZobristHash(&position);

    if (position.hash != computedHash)
    {
        printf("\n");
        printf(
            "!!! ERREUR HASH POSITION INITIALE !!!\n"
        );

        printf(
            "Position.hash  : %llu\n",
            (unsigned long long)position.hash
        );

        printf(
            "Computed hash  : %llu\n",
            (unsigned long long)computedHash
        );

        return;
    }

    printf(
        "\nHash position : %llu\n",
        (unsigned long long)position.hash
    );

    /*
     * --------------------------------------------------------
     * RECHERCHE
     * --------------------------------------------------------
     */

    SearchResult result =
        IterativeDeepening(
            &position,
            maxDepth,
            thinkTimeSeconds,
            NULL
        );

    /*
     * --------------------------------------------------------
     * RESULTAT
     * --------------------------------------------------------
     */

    printf("\n");
    printf("----------------------------------------\n");

    printf("Meilleur coup trouve : ");

    PrintMove(result.move);

    printf("\n");

    printf(
        "Evaluation          : %d\n",
        result.score
    );

    printf(
        "Profondeur atteinte : %d\n",
        result.depth
    );

    /*
     * --------------------------------------------------------
     * STATISTIQUES TT
     * --------------------------------------------------------
     */

    printf("\n");
    printf("STATISTIQUES TRANSPOSITION TABLE\n");

    TT_PrintStats();

    printf("----------------------------------------\n");
}


/* ============================================================
   MAIN
   ============================================================ */

int main(int argc, char *argv[])
{

    InitZobrist();
    TT_Init(64);

    //if (argc > 1 &&
    //strcmp(argv[1], "--uci") == 0)
    //{
        UCI_Loop();
        return 0;
    //}

    printf("\n");
    printf("========================================\n");
    printf("          CHESSBOT - TESTS\n");
    printf("========================================\n");

    /*
     * --------------------------------------------------------
     * INITIALISATION
     * --------------------------------------------------------
     */



    double thinkTimeSeconds = 5.0;

    int maxDepth = 100;


    /*
     * ========================================================
     * TEST 1 : MAT EN 1
     * ========================================================
     */

    /*TestPositionFromFEN(
        "7k/6Q1/6K1/8/8/8/8/8 w - - 0 1",
        "1 - MAT EN 1",
        thinkTimeSeconds,
        maxDepth
    );*/

    /*TestPositionFromFEN(
        "3r2k1/p4ppp/8/Q2r1q2/4p3/8/PP3PPP/3R1RK1 w - - 0 21",
        "Probleme lichess 1",
        thinkTimeSeconds,
        maxDepth
    );

        TestPositionFromFEN(
        "r2qk2r/pp1np1bp/2p1Q1p1/4P1B1/3nN1P1/8/PP2BP1P/R4RK1 w kq - 0 18",
        "Probleme lichess 2",
        thinkTimeSeconds,
        maxDepth
    );

    TestPositionFromFEN(
        "6k1/5q1p/6p1/p2Q2P1/2n4P/8/6BK/8 w - - 0 45",
        "Probleme lichess 3",
        thinkTimeSeconds,
        maxDepth
    );

    TestPositionFromFEN(
        "4Q3/1p5k/6p1/1p1b1rqp/1Pp3N1/P6P/5PP1/4R1K1 w - - 0 30",
        "Probleme lichess 4",
        thinkTimeSeconds,
        maxDepth
    );*/
/*

    TestPositionFromFEN(
        "6k1/5ppp/8/8/8/8/5Q2/6K1 w - - 0 1",
        "2 - FINALE DAME / RECHERCHE DE MAT",
        thinkTimeSeconds,
        maxDepth
    );



    TestPositionFromFEN(
        "r3k2r/ppp2ppp/2n5/3q4/8/4N3/PPPP1PPP/R2QK2R w KQkq - 0 1",
        "3 - GAIN DE MATERIEL",
        thinkTimeSeconds,
        maxDepth
    );




    TestPositionFromFEN(
        "r1bqk2r/pppp1ppp/2n2n2/8/2B1P3/2N2N2/PPPP1PPP/R1BQK2R w KQkq - 0 1",
        "4 - MILIEU DE JEU / TACTIQUE",
        thinkTimeSeconds,
        maxDepth
    );



    TestPositionFromFEN(
        "k7/P7/8/8/8/8/8/6K1 w - - 0 1",
        "5 - FINALE NULLE",
        thinkTimeSeconds,
        maxDepth
    );

    TestPositionFromFEN(
        "4k3/8/8/3pP3/8/8/8/4K3 w - d6 0 1",
        "6 - PRISE EN PASSANT",
        thinkTimeSeconds,
        maxDepth
    );



    TestPositionFromFEN(
        "r3k2r/pppq1ppp/2npbn2/8/2B1P3/2N1BN2/PPP2PPP/R2QK2R w KQkq - 0 1",
        "7 - MILIEU DE JEU / ROQUE",
        thinkTimeSeconds,
        maxDepth
    );



    TestPositionFromFEN(
        "r1bq1rk1/ppp2ppp/2np1n2/8/2B1P3/2N1BN2/PPP2PPP/R2Q1RK1 w - - 0 1",
        "8 - MILIEU DE JEU COMPLEXE",
        thinkTimeSeconds,
        maxDepth
    );*/


    /*
     * ========================================================
     * FIN
     * ========================================================
     */

    printf("\n");
    printf("========================================\n");
    printf("             TESTS TERMINES\n");
    printf("========================================\n\n");

    return 0;
}