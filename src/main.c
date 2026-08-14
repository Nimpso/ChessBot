#include <stdio.h>
#include <time.h>

#include "board.h"
#include "move.h"
#include "eval.h"
#include "search.h"


/* ============================================================
   OUTILS D'AFFICHAGE
   ============================================================ */

void PrintMove(Move move)
{
    printf("%c%d -> %c%d",
           'a' + move.fromCol,
           8 - move.fromRow,
           'a' + move.toCol,
           8 - move.toRow);

    if (move.promotion != '\0')
    {
        printf("=%c", move.promotion);
    }

    printf("\n");
}


void PrintMoveList(MoveList *moveList)
{
    for (int i = 0; i < moveList->count; i++)
    {
        printf("%3d. ", i + 1);
        PrintMove(moveList->moves[i]);
    }
}


/* ============================================================
   TEST 1
   POSITION INITIALE
   ============================================================ */

void TestMoveGeneration(void)
{
    Position position;
    MoveList moveList;

    InitBoard(&position);

    printf("\n");
    printf("========================================\n");
    printf("     TEST 1 : POSITION INITIALE\n");
    printf("========================================\n\n");

    PrintBoard(&position);

    moveList.count = 0;


    /* --------------------------------------------------------
       PIONS
       -------------------------------------------------------- */

    printf("\n--- Pions ---\n");

    GeneratePawnMoves(&position, &moveList);

    printf("Nombre de coups : %d\n", moveList.count);

    if (moveList.count == 16)
        printf("[PASS] 16 coups de pions\n");
    else
        printf("[FAIL] Attendu : 16\n");


    /* --------------------------------------------------------
       CAVALIERS
       -------------------------------------------------------- */

    printf("\n--- Cavaliers ---\n");

    GenerateKnightMoves(&position, &moveList);

    printf("Nombre de coups : %d\n", moveList.count);

    if (moveList.count == 20)
        printf("[PASS] 16 pions + 4 cavaliers = 20\n");
    else
        printf("[FAIL] Attendu : 20\n");


    /* --------------------------------------------------------
       FOUS
       -------------------------------------------------------- */

    printf("\n--- Fous ---\n");

    GenerateBishopMoves(&position, &moveList);

    printf("Nombre de coups : %d\n", moveList.count);

    if (moveList.count == 20)
        printf("[PASS] Les fous sont bloques\n");
    else
        printf("[FAIL] Les fous ne devraient pas bouger\n");


    /* --------------------------------------------------------
       TOURS
       -------------------------------------------------------- */

    printf("\n--- Tours ---\n");

    GenerateRookMoves(&position, &moveList);

    printf("Nombre de coups : %d\n", moveList.count);

    if (moveList.count == 20)
        printf("[PASS] Les tours sont bloquees\n");
    else
        printf("[FAIL] Les tours ne devraient pas bouger\n");


    /* --------------------------------------------------------
       DAMES
       -------------------------------------------------------- */

    printf("\n--- Dames ---\n");

    GenerateQueenMoves(&position, &moveList);

    printf("Nombre de coups : %d\n", moveList.count);

    if (moveList.count == 20)
        printf("[PASS] Les dames sont bloquees\n");
    else
        printf("[FAIL] Les dames ne devraient pas bouger\n");


    /* --------------------------------------------------------
       ROIS
       -------------------------------------------------------- */

    printf("\n--- Rois ---\n");

    GenerateKingMoves(&position, &moveList);

    printf("Nombre de coups : %d\n", moveList.count);

    if (moveList.count == 20)
        printf("[PASS] Les rois sont bloques\n");
    else
        printf("[FAIL] Les rois ne devraient pas bouger\n");


    printf("\nListe finale :\n\n");

    PrintMoveList(&moveList);
}


/* ============================================================
   TEST 2
   ACCUMULATION DES GENERATEURS
   ============================================================ */

void TestMoveListAccumulation(void)
{
    Position position;
    MoveList moveList;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            position.board[i][j] = '.';
        }
    }

    position.sideToMove = 1;

    position.whiteKingSideCastle = 0;
    position.whiteQueenSideCastle = 0;
    position.blackKingSideCastle = 0;
    position.blackQueenSideCastle = 0;

    position.enPassantRow = -1;
    position.enPassantCol = -1;
    position.halfMoveClock = 0;


    /* Pieces blanches */

    position.board[7][4] = 'K';   // e1
    position.board[7][5] = 'R';   // f1
    position.board[3][3] = 'B';   // d5
    position.board[4][6] = 'N';   // g4


    /* Roi noir */

    position.board[0][4] = 'k';   // e8


    /* --------------------------------------------------------
       AFFICHAGE
       -------------------------------------------------------- */

    printf("\n");
    printf("========================================\n");
    printf(" TEST 2 : GENERATE PSEUDO-LEGALMOVES\n");
    printf("========================================\n\n");

    PrintBoard(&position);
    GeneratePseudoLegalMoves(&position, &moveList);

    /* --------------------------------------------------------
       LISTE COMPLETE
       -------------------------------------------------------- */

    printf("========================================\n");
    printf("       LISTE COMPLETE DES COUPS\n");
    printf("========================================\n\n");

    printf("Nombre total : %d\n\n", moveList.count);

    PrintMoveList(&moveList);


    printf("\n========================================\n");
    printf("           FIN DU TEST 2\n");
    printf("========================================\n");
}


/* ============================================================
   TEST 3
   PERFT - POSITION INITIALE
   ============================================================ */

void TestPerftInitialPosition(void)
{
    Position position;
    InitBoard(&position);

    long long expected[] = {1, 20, 400, 8902, 197281, 4865609};

    printf("\n");
    printf("========================================\n");
    printf("   TEST 3 : PERFT - POSITION INITIALE\n");
    printf("========================================\n\n");

    for (int depth = 1; depth <= 5; depth++)
    {
        long long nodes = Perft(&position, depth);

        printf("Perft(%d) = %lld  (attendu : %lld)  %s\n",
               depth, nodes, expected[depth],
               nodes == expected[depth] ? "[PASS]" : "[FAIL]");
    }
}


/* ============================================================
   TEST 4
   PERFT - POSITION "KIWIPETE"

   Position de référence standard pour piéger les bugs de
   roque à travers l'échec, de clouages, et de prises en
   passant. FEN :
   r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R
   w KQkq - 0 1
   ============================================================ */

void SetupKiwipetePosition(Position *position)
{
    const char *rows[8] =
    {
        "r...k..r",
        "p.ppqpb.",
        "bn..pnp.",
        "...PN...",
        ".p..P...",
        "..N..Q.p",
        "PPPBBPPP",
        "R...K..R"
    };

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            position->board[i][j] = rows[i][j];
        }
    }

    position->sideToMove = 0;

    position->whiteKingSideCastle  = 1;
    position->whiteQueenSideCastle = 1;
    position->blackKingSideCastle  = 1;
    position->blackQueenSideCastle = 1;

    position->enPassantRow = -1;
    position->enPassantCol = -1;
    position->halfMoveClock = 0;
}

void TestPerftKiwipete(void)
{
    Position position;
    SetupKiwipetePosition(&position);

    long long expected[] = {1, 48, 2039, 97862, 4085603};

    printf("\n");
    printf("========================================\n");
    printf("      TEST 4 : PERFT - KIWIPETE\n");
    printf("========================================\n\n");

    PrintBoard(&position);
    printf("\n");

    for (int depth = 1; depth <= 4; depth++)
    {
        long long nodes = Perft(&position, depth);

        printf("Perft(%d) = %lld  (attendu : %lld)  %s\n",
               depth, nodes, expected[depth],
               nodes == expected[depth] ? "[PASS]" : "[FAIL]");
    }
}


/* ============================================================
   TEST 5
   EVALUATE SUR 5 POSITIONS DIFFERENTES
   ============================================================ */

void SetupPositionFromRows(Position *position, const char *rows[8],
                            int sideToMove,
                            int wk, int wq, int bk, int bq)
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            position->board[i][j] = rows[i][j];

    position->sideToMove = sideToMove;

    position->whiteKingSideCastle  = wk;
    position->whiteQueenSideCastle = wq;
    position->blackKingSideCastle  = bk;
    position->blackQueenSideCastle = bq;

    position->enPassantRow = -1;
    position->enPassantCol = -1;
    position->halfMoveClock = 0;
}

void ShowAndEvaluate(const char *title, Position *position)
{
    printf("\n----------------------------------------\n");
    printf(" %s\n", title);
    printf("----------------------------------------\n\n");

    PrintBoard(position);

    int score = Evaluate(position);
    int whiteInCheck = IsInCheck(position, 0);
    int blackInCheck = IsInCheck(position, 1);
    int mate = IsCheckmate(position);
    int stalemate = IsStalemate(position);

    printf("\nTrait aux : %s\n", position->sideToMove == 0 ? "Blancs" : "Noirs");
    printf("Blancs en echec : %s\n", whiteInCheck ? "oui" : "non");
    printf("Noirs en echec  : %s\n", blackInCheck ? "oui" : "non");
    printf("Mat             : %s\n", mate ? "oui" : "non");
    printf("Pat             : %s\n", stalemate ? "oui" : "non");
    printf("Evaluate()      : %d\n", score);
}

void TestEvaluationExamples(void)
{
    Position position;

    printf("\n");
    printf("========================================\n");
    printf("   TEST 5 : EVALUATE SUR 5 POSITIONS\n");
    printf("========================================\n");


    /* POSITION 1 : position initiale (equilibree) */

    InitBoard(&position);
    ShowAndEvaluate("POSITION 1 : Position initiale", &position);


    /* POSITION 2 : les blancs ont une dame de plus */

    InitBoard(&position);
    position.board[0][3] = '.'; // dame noire retiree
    ShowAndEvaluate("POSITION 2 : Blancs +Dame", &position);


    /* POSITION 3 : mat du berger (les noirs sont mates) */

    {
        const char *rows[8] = {
            "r.bqkb.r",
            "pppp.Qpp",
            "..n.....",
            "....p...",
            "..B.P...",
            "........",
            "PPPP.PPP",
            "RNB.K.NR"
        };
        SetupPositionFromRows(&position, rows, 1, 0, 0, 0, 0);
    }
    ShowAndEvaluate("POSITION 3 : Mat du berger", &position);


    /* POSITION 4 : pat (roi a1 blanc bloque) */

    {
        const char *rows[8] = {
            "........",
            "........",
            "........",
            "........",
            "........",
            "kq......",
            "........",
            "K......."
        };
        SetupPositionFromRows(&position, rows, 0, 0, 0, 0, 0);
    }
    ShowAndEvaluate("POSITION 4 : Pat", &position);


    /* POSITION 5 : finale Tour+pion vs Fou+pion */

    {
        const char *rows[8] = {
            "..b.k...",
            "........",
            "........",
            "....p...",
            "...P....",
            "........",
            "........",
            "R...K..."
        };
        SetupPositionFromRows(&position, rows, 0, 0, 0, 0, 0);
    }
    ShowAndEvaluate("POSITION 5 : Finale Tour+pion vs Fou+pion", &position);
}


/* ============================================================
   TEST 6
   SELF-PLAY : LE MOTEUR JOUE CONTRE LUI-MEME
   ============================================================ */

void TestSelfPlay(int maxHalfMoves, int depth)
{
    Position position;
    InitBoard(&position);

    printf("\n");
    printf("========================================\n");
    printf("   TEST 6 : SELF-PLAY (profondeur %d)\n", depth);
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

        Move best = FindBestMove(&position, depth);

        printf("%3d. %s joue : %c%d -> %c%d",
               ply,
               position.sideToMove == 0 ? "Blancs" : "Noirs ",
               'a' + best.fromCol, 8 - best.fromRow,
               'a' + best.toCol,   8 - best.toRow);

        if (best.promotion != '\0')
        {
            printf("=%c", best.promotion);
        }

        MakeMove(&position, best);

        printf("   (score apres coup : %d)\n", Evaluate(&position));
    }

    printf("\n>>> Limite de %d demi-coups atteinte, partie non terminee.\n",
           maxHalfMoves);
    printf("\n");
    PrintBoard(&position);
}


/* ============================================================
   TEST 7
   MINIMAX VS ALPHABETA
   ============================================================ */

void TestAlphaBetaComparison(void)
{
    Position position;
    InitBoard(&position);

    printf("\n");
    printf("========================================\n");
    printf("   TEST 7 : MINIMAX VS ALPHABETA\n");
    printf("========================================\n\n");

    for (int depth = 1; depth <= 4; depth++)
    {
        int minimaxScore = Minimax(&position, depth, 1);
        int alphaBetaScore = AlphaBeta(&position, depth, -2000000000, 2000000000, 1);

        printf("Profondeur %d : Minimax = %d, AlphaBeta = %d  %s\n",
               depth, minimaxScore, alphaBetaScore,
               minimaxScore == alphaBetaScore ? "[PASS]" : "[FAIL]");
    }

    printf("\n--- Temps d'execution ---\n\n");

    for (int depth = 1; depth <= 5; depth++)
    {
        clock_t start = clock();
        Minimax(&position, depth, 1);
        double minimaxTime = (double)(clock() - start) / CLOCKS_PER_SEC;

        start = clock();
        AlphaBeta(&position, depth, -2000000000, 2000000000, 1);
        double alphaBetaTime = (double)(clock() - start) / CLOCKS_PER_SEC;

        printf("Profondeur %d : Minimax = %.3fs, AlphaBeta = %.3fs\n",
               depth, minimaxTime, alphaBetaTime);
    }
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


    /*
     * Test 1 :
     * vérifie que les générateurs peuvent
     * s'enchaîner sur la position initiale.
     */

    //TestMoveGeneration();


    /*
     * Test 2 :
     * vérifie réellement que les générateurs
     * ajoutent leurs coups à la même MoveList
     * sans effacer les précédents.
     */

    //TestMoveListAccumulation();


    /*
     * Test 3 :
     * Perft sur la position initiale, jusqu'à
     * profondeur 5. Valide pions/pièces/roque/
     * en passant/promotion/légalité ensemble.
     */

    //TestPerftInitialPosition();


    /*
     * Test 4 :
     * Perft sur Kiwipete, la position de référence
     * qui piège les bugs de roque à travers l'échec,
     * de clouages, et de prises en passant.
     */

    //TestPerftKiwipete();


    /*
     * Test 5 :
     * Evaluate() sur 5 positions differentes, pour
     * valider l'evaluation materielle et la detection
     * de mat/pat au passage.
     */

    //TestEvaluationExamples();


    /*
     * Test 6 :
     * le moteur joue contre lui-meme sur quelques coups,
     * avec FindBestMove(). Profondeur 3 pour rester rapide
     * (Minimax sans Alpha-Beta grossit vite).
     */
    //TestSelfPlay(500, 3);

    // Test 7 : verifie que AlphaBeta donne le meme resultat que Minimax, en plus rapide
    TestAlphaBetaComparison();





    printf("\n");
    printf("========================================\n");
    printf("             TESTS TERMINES\n");
    printf("========================================\n\n");

    return 0;
}