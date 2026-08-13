#include <stdio.h>

#include "board.h"
#include "move.h"


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
}

void TestPerftKiwipete(void)
{
    Position position;
    SetupKiwipetePosition(&position);

    long long expected[] = {1, 48, 2039, 97862, 4085603, 193690690};

    printf("\n");
    printf("========================================\n");
    printf("      TEST 4 : PERFT - KIWIPETE\n");
    printf("========================================\n\n");

    PrintBoard(&position);
    printf("\n");

    for (int depth = 1; depth <= 5; depth++)
    {
        long long nodes = Perft(&position, depth);

        printf("Perft(%d) = %lld  (attendu : %lld)  %s\n",
               depth, nodes, expected[depth],
               nodes == expected[depth] ? "[PASS]" : "[FAIL]");
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

    TestPerftInitialPosition();


    /*
     * Test 4 :
     * Perft sur Kiwipete, la position de référence
     * qui piège les bugs de roque à travers l'échec,
     * de clouages, et de prises en passant.
     */

    TestPerftKiwipete();


    printf("\n");
    printf("========================================\n");
    printf("             TESTS TERMINES\n");
    printf("========================================\n\n");

    return 0;
}