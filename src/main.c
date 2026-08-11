#include <stdio.h>

#include "board.h"
#include "move.h"


int testsPassed = 0;
int testsFailed = 0;


/* ============================================================
   OUTILS
   ============================================================ */

void Check(int condition, const char *testName)
{
    if (condition)
    {
        printf("[PASS] %s\n", testName);
        testsPassed++;
    }
    else
    {
        printf("[FAIL] %s\n", testName);
        testsFailed++;
    }
}


void ClearBoard(Position *position)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            position->board[i][j] = '.';
        }
    }

    position->sideToMove = 0;

    position->whiteKingSideCastle = 0;
    position->whiteQueenSideCastle = 0;
    position->blackKingSideCastle = 0;
    position->blackQueenSideCastle = 0;
}


/* ============================================================
   TEST 1 : POSITION VIDE AVEC LES DEUX ROIS
   ============================================================ */

void TestNoCheck(void)
{
    Position position;

    printf("\n");
    printf("============================================================\n");
    printf("TEST 1 : AUCUN ECHEC\n");
    printf("============================================================\n");

    ClearBoard(&position);

    position.board[7][4] = 'K'; // e1
    position.board[0][4] = 'k'; // e8

    Check(!IsInCheck(&position, 0),
          "Roi blanc non attaque");

    Check(!IsInCheck(&position, 1),
          "Roi noir non attaque");
}


/* ============================================================
   TEST 2 : ECHEC PAR PION
   ============================================================ */

void TestPawnCheck(void)
{
    Position position;

    printf("\n");
    printf("============================================================\n");
    printf("TEST 2 : ECHEC PAR PION\n");
    printf("============================================================\n");


    /* --------------------------------------------------------
       Pion noir attaque roi blanc
       -------------------------------------------------------- */

    ClearBoard(&position);

    position.board[7][4] = 'K'; // e1
    position.board[6][3] = 'p'; // d2

    Check(IsInCheck(&position, 0),
          "Roi blanc en e1 attaque par pion noir");


    /* --------------------------------------------------------
       Pion blanc attaque roi noir
       -------------------------------------------------------- */

    ClearBoard(&position);

    position.board[0][4] = 'k'; // e8
    position.board[1][3] = 'P'; // d7

    Check(IsInCheck(&position, 1),
          "Roi noir en e8 attaque par pion blanc");


    /* --------------------------------------------------------
       Pion qui n'attaque PAS le roi
       -------------------------------------------------------- */

    ClearBoard(&position);

    position.board[7][4] = 'K'; // e1
    position.board[6][4] = 'p'; // e2

    Check(!IsInCheck(&position, 0),
          "Pion devant le roi mais sans attaque");
}


/* ============================================================
   TEST 3 : ECHEC PAR CAVALIER
   ============================================================ */

void TestKnightCheck(void)
{
    Position position;

    printf("\n");
    printf("============================================================\n");
    printf("TEST 3 : ECHEC PAR CAVALIER\n");
    printf("============================================================\n");


    /* Cavalier noir attaque e1 depuis d3 */

    ClearBoard(&position);

    position.board[7][4] = 'K'; // e1
    position.board[5][3] = 'n'; // d3

    Check(IsInCheck(&position, 0),
          "Roi blanc attaque par cavalier noir");


    /* Cavalier blanc attaque e8 depuis d6 */

    ClearBoard(&position);

    position.board[0][4] = 'k'; // e8
    position.board[2][3] = 'N'; // d6

    Check(IsInCheck(&position, 1),
          "Roi noir attaque par cavalier blanc");


    /* Cavalier trop loin */

    ClearBoard(&position);

    position.board[7][4] = 'K'; // e1
    position.board[5][4] = 'n'; // e3

    Check(!IsInCheck(&position, 0),
          "Cavalier non attaquant");
}


/* ============================================================
   TEST 4 : ECHEC PAR FOU
   ============================================================ */

void TestBishopCheck(void)
{
    Position position;

    printf("\n");
    printf("============================================================\n");
    printf("TEST 4 : ECHEC PAR FOU\n");
    printf("============================================================\n");


    /* Fou noir d5 -> e4 -> f3 -> g2 -> h1 */

    ClearBoard(&position);

    position.board[7][4] = 'K'; // e1
    position.board[4][1] = 'b'; // b5

    Check(IsInCheck(&position, 0),
          "Roi blanc attaque par fou noir");


    /* Fou blanc b4 -> e7 */

    ClearBoard(&position);

    position.board[0][4] = 'k'; // e8
    position.board[3][1] = 'B'; // b5

    Check(IsInCheck(&position, 1),
          "Roi noir attaque par fou blanc");


    /* Fou bloqué */

    ClearBoard(&position);

    position.board[7][4] = 'K'; // e1
    position.board[4][1] = 'b'; // b4
    position.board[5][2] = 'P'; // c3

    PrintBoard(&position);

    Check(!IsInCheck(&position, 0),
          "Fou bloque avant le roi");
}


/* ============================================================
   TEST 5 : ECHEC PAR TOUR
   ============================================================ */

void TestRookCheck(void)
{
    Position position;

    printf("\n");
    printf("============================================================\n");
    printf("TEST 5 : ECHEC PAR TOUR\n");
    printf("============================================================\n");


    /* Tour noire sur e8 -> roi blanc e1 */

    ClearBoard(&position);

    position.board[7][4] = 'K'; // e1
    position.board[0][4] = 'r'; // e8

    Check(IsInCheck(&position, 0),
          "Roi blanc attaque par tour noire");


    /* Tour blanche sur e1 -> roi noir e8 */

    ClearBoard(&position);

    position.board[0][4] = 'k'; // e8
    position.board[7][4] = 'R'; // e1

    Check(IsInCheck(&position, 1),
          "Roi noir attaque par tour blanche");


    /* Tour bloquée */

    ClearBoard(&position);

    position.board[7][4] = 'K'; // e1
    position.board[0][4] = 'r'; // e8
    position.board[5][4] = 'N'; // e3

    Check(!IsInCheck(&position, 0),
          "Tour bloquee par une piece");
}


/* ============================================================
   TEST 6 : ECHEC PAR DAME
   ============================================================ */

void TestQueenCheck(void)
{
    Position position;

    printf("\n");
    printf("============================================================\n");
    printf("TEST 6 : ECHEC PAR DAME\n");
    printf("============================================================\n");


    /* Dame noire sur h5 -> roi blanc e2 */

    ClearBoard(&position);

    position.board[6][4] = 'K'; // e2
    position.board[3][7] = 'q'; // h5

    Check(IsInCheck(&position, 0),
          "Roi blanc attaque par dame noire");


    /* Dame blanche sur h4 -> roi noir e7 */

    ClearBoard(&position);

    position.board[1][4] = 'k'; // e7
    position.board[4][7] = 'Q'; // h4

    Check(IsInCheck(&position, 1),
          "Roi noir attaque par dame blanche");


    /* Dame bloquée */

    ClearBoard(&position);

    position.board[6][4] = 'K'; // e2
    position.board[3][7] = 'q'; // h5
    position.board[5][5] = 'N'; // f3

    Check(!IsInCheck(&position, 0),
          "Dame bloquee par une piece");
}


/* ============================================================
   TEST 7 : ECHEC PAR ROI
   ============================================================ */

void TestKingCheck(void)
{
    Position position;

    printf("\n");
    printf("============================================================\n");
    printf("TEST 7 : ECHEC PAR ROI\n");
    printf("============================================================\n");


    /* Deux rois adjacents */

    ClearBoard(&position);

    position.board[7][4] = 'K'; // e1
    position.board[6][4] = 'k'; // e2

    Check(IsInCheck(&position, 0),
          "Roi blanc attaque par roi noir");

    Check(IsInCheck(&position, 1),
          "Roi noir attaque par roi blanc");


    /* Deux rois éloignés */

    ClearBoard(&position);

    position.board[7][4] = 'K'; // e1
    position.board[0][4] = 'k'; // e8

    Check(!IsInCheck(&position, 0),
          "Rois eloignes : blanc pas en echec");

    Check(!IsInCheck(&position, 1),
          "Rois eloignes : noir pas en echec");
}


/* ============================================================
   TEST 8 : PIECE AMIE QUI BLOQUE
   ============================================================ */

void TestFriendlyBlock(void)
{
    Position position;

    printf("\n");
    printf("============================================================\n");
    printf("TEST 8 : BLOCAGE PAR PIECE ALLIEE\n");
    printf("============================================================\n");


    /*
       Tour noire en e8
       Roi blanc en e1
       Pièce blanche en e4

       e8 -> e7 -> e6 -> e5 -> e4
                                  X
                              blocage
    */

    ClearBoard(&position);

    position.board[7][4] = 'K'; // e1
    position.board[0][4] = 'r'; // e8
    position.board[4][4] = 'N'; // e4

    Check(!IsInCheck(&position, 0),
          "Piece blanche bloque la tour noire");


    /*
       Fou noir bloqué par une pièce blanche
    */

    ClearBoard(&position);

    position.board[7][4] = 'K'; // e1
    position.board[4][1] = 'b'; // b5
    position.board[5][2] = 'N'; // c3

    PrintBoard(&position);

    Check(!IsInCheck(&position, 0),
          "Piece blanche bloque le fou noir");
}


/* ============================================================
   TEST 9 : PIECE ADVERSE ENTRE L'ATTAQUANT ET LE ROI
   ============================================================ */

void TestEnemyBlock(void)
{
    Position position;

    printf("\n");
    printf("============================================================\n");
    printf("TEST 9 : PIECE ADVERSE ENTRE ATTAQUANT ET ROI\n");
    printf("============================================================\n");


    /*
       Tour noire en e8
       Roi blanc en e1
       Pièce noire en e4

       La tour est bloquée par sa propre pièce.
    */

    ClearBoard(&position);

    position.board[7][4] = 'K'; // e1
    position.board[0][4] = 'r'; // e8
    position.board[4][4] = 'p'; // e4

    Check(!IsInCheck(&position, 0),
          "Piece noire bloque sa propre tour");


    /*
       Fou noir bloqué par une pièce noire
    */

    ClearBoard(&position);

    position.board[7][4] = 'K'; // e1
    position.board[4][1] = 'b'; // b5
    position.board[5][2] = 'n'; // c3

    PrintBoard(&position);

    Check(!IsInCheck(&position, 0),
          "Piece noire bloque son propre fou");
}


/* ============================================================
   TEST 10 : POSITION REELLE
   ============================================================ */

void TestRealPosition(void)
{
    Position position;

    printf("\n");
    printf("============================================================\n");
    printf("TEST 10 : POSITION INITIALE REELLE\n");
    printf("============================================================\n");


    InitBoard(&position);

    Check(!IsInCheck(&position, 0),
          "Position initiale : blancs pas en echec");

    Check(!IsInCheck(&position, 1),
          "Position initiale : noirs pas en echec");
}


/* ============================================================
   MAIN
   ============================================================ */

int main(void)
{
    printf("\n");
    printf("============================================================\n");
    printf("              CHESSBOT - TEST IsInCheck()\n");
    printf("============================================================\n");


    TestNoCheck();

    TestPawnCheck();

    TestKnightCheck();

    TestBishopCheck();

    TestRookCheck();

    TestQueenCheck();

    TestKingCheck();

    TestFriendlyBlock();

    TestEnemyBlock();

    TestRealPosition();


    printf("\n");
    printf("============================================================\n");
    printf("                    RESULTATS\n");
    printf("============================================================\n");

    printf("\nTests reussis : %d\n", testsPassed);
    printf("Tests echoues : %d\n", testsFailed);
    printf("Total         : %d\n",
           testsPassed + testsFailed);


    if (testsFailed == 0)
    {
        printf("\n>>> TOUS LES TESTS PASSENT <<<\n");
        return 0;
    }

    printf("\n>>> DES TESTS ONT ECHOUE <<<\n");
    return 1;
}