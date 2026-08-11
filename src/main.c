#include <stdio.h>

#include "board.h"
#include "move.h"


/* ============================================================
   OUTILS D'AFFICHAGE
   ============================================================ */

void PrintMove(Move move)
{
    printf("%c%d -> %c%d\n",
           'a' + move.fromCol,
           8 - move.fromRow,
           'a' + move.toCol,
           8 - move.toRow);
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

    TestMoveGeneration();


    /*
     * Test 2 :
     * vérifie réellement que les générateurs
     * ajoutent leurs coups à la même MoveList
     * sans effacer les précédents.
     */

    TestMoveListAccumulation();


    printf("\n");
    printf("========================================\n");
    printf("             TESTS TERMINES\n");
    printf("========================================\n\n");

    return 0;
}
