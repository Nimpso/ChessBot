#include <stdio.h>
#include <string.h>

#include "board.h"
#include "move.h"


/* ============================================================
   OUTILS DE TEST
   ============================================================ */

int testsPassed = 0;
int testsFailed = 0;


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
}


void ResetEmptyPosition(Position *position)
{
    ClearBoard(position);

    position->sideToMove = 0;

    position->whiteKingSideCastle = 0;
    position->whiteQueenSideCastle = 0;
    position->blackKingSideCastle = 0;
    position->blackQueenSideCastle = 0;
}


int HasMove(MoveList *moveList,
            int fromRow,
            int fromCol,
            int toRow,
            int toCol)
{
    for (int i = 0; i < moveList->count; i++)
    {
        Move move = moveList->moves[i];

        if (move.fromRow == fromRow &&
            move.fromCol == fromCol &&
            move.toRow == toRow &&
            move.toCol == toCol)
        {
            return 1;
        }
    }

    return 0;
}


int HasMoveChessNotation(MoveList *moveList,
                         char fromFile,
                         int fromRank,
                         char toFile,
                         int toRank)
{
    int fromCol = fromFile - 'a';
    int fromRow = 8 - fromRank;

    int toCol = toFile - 'a';
    int toRow = 8 - toRank;

    return HasMove(moveList,
                   fromRow,
                   fromCol,
                   toRow,
                   toCol);
}


void PrintMoves(MoveList *moveList)
{
    for (int i = 0; i < moveList->count; i++)
    {
        Move move = moveList->moves[i];

        printf("%2d. %c%d -> %c%d\n",
               i + 1,
               'a' + move.fromCol,
               8 - move.fromRow,
               'a' + move.toCol,
               8 - move.toRow);
    }
}


void PrintTestHeader(const char *name)
{
    printf("\n");
    printf("============================================================\n");
    printf("%s\n", name);
    printf("============================================================\n\n");
}


/* ============================================================
   TESTS PIONS
   ============================================================ */

void TestPawns(void)
{
    Position position;
    MoveList moveList;

    PrintTestHeader("TESTS PIONS");


    /* --------------------------------------------------------
       Test 1 : position initiale
       -------------------------------------------------------- */

    InitBoard(&position);

    GeneratePawnMoves(&position, &moveList);

    Check(moveList.count == 16,
          "Position initiale : 16 coups de pions blancs");


    /* --------------------------------------------------------
       Test 2 : pion blanc avance d'une case
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[6][4] = 'P'; // e2

    GeneratePawnMoves(&position, &moveList);

    Check(moveList.count == 2,
          "Pion blanc e2 : 2 coups possibles");

    Check(HasMoveChessNotation(&moveList, 'e', 2, 'e', 3),
          "Pion blanc e2 -> e3");

    Check(HasMoveChessNotation(&moveList, 'e', 2, 'e', 4),
          "Pion blanc e2 -> e4");


    /* --------------------------------------------------------
       Test 3 : pion blanc bloqué
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[6][4] = 'P';
    position.board[5][4] = 'p';

    GeneratePawnMoves(&position, &moveList);

    Check(moveList.count == 0,
          "Pion blanc bloqué : aucun déplacement");


    /* --------------------------------------------------------
       Test 4 : capture blanche à droite
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'P'; // e4
    position.board[3][5] = 'p'; // f5

    GeneratePawnMoves(&position, &moveList);

    Check(HasMoveChessNotation(&moveList, 'e', 4, 'f', 5),
          "Pion blanc capture en diagonale droite");


    /* --------------------------------------------------------
       Test 5 : capture blanche à gauche
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'P'; // e4
    position.board[3][3] = 'p'; // d5

    GeneratePawnMoves(&position, &moveList);

    Check(HasMoveChessNotation(&moveList, 'e', 4, 'd', 5),
          "Pion blanc capture en diagonale gauche");


    /* --------------------------------------------------------
       Test 6 : pas de capture sur pièce blanche
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'P';
    position.board[3][5] = 'N';

    GeneratePawnMoves(&position, &moveList);

    Check(!HasMoveChessNotation(&moveList, 'e', 4, 'f', 5),
          "Pion blanc ne capture pas une pièce blanche");


    /* --------------------------------------------------------
       Test 7 : pion noir
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.sideToMove = 1;

    position.board[1][4] = 'p'; // e7

    GeneratePawnMoves(&position, &moveList);

    Check(moveList.count == 2,
          "Pion noir e7 : 2 coups possibles");

    Check(HasMoveChessNotation(&moveList, 'e', 7, 'e', 6),
          "Pion noir e7 -> e6");

    Check(HasMoveChessNotation(&moveList, 'e', 7, 'e', 5),
          "Pion noir e7 -> e5");


    /* --------------------------------------------------------
       Test 8 : capture noire
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.sideToMove = 1;

    position.board[3][4] = 'p'; // e5
    position.board[4][5] = 'P'; // f4

    GeneratePawnMoves(&position, &moveList);

    Check(HasMoveChessNotation(&moveList, 'e', 5, 'f', 4),
          "Pion noir capture une pièce blanche");


    /* --------------------------------------------------------
       Test 9 : pion noir bloqué
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.sideToMove = 1;

    position.board[1][4] = 'p';
    position.board[2][4] = 'P';

    GeneratePawnMoves(&position, &moveList);

    Check(moveList.count == 0,
          "Pion noir bloqué : aucun déplacement");
}


/* ============================================================
   TESTS CAVALIERS
   ============================================================ */

void TestKnights(void)
{
    Position position;
    MoveList moveList;

    PrintTestHeader("TESTS CAVALIERS");


    /* --------------------------------------------------------
       Test 1 : cavalier au centre
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'N'; // e4

    GenerateKnightMoves(&position, &moveList);

    Check(moveList.count == 8,
          "Cavalier blanc e4 : 8 coups");


    /* --------------------------------------------------------
       Test 2 : coin
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[7][0] = 'N'; // a1

    GenerateKnightMoves(&position, &moveList);

    Check(moveList.count == 2,
          "Cavalier a1 : 2 coups");

    Check(HasMoveChessNotation(&moveList, 'a', 1, 'b', 3),
          "Cavalier a1 -> b3");

    Check(HasMoveChessNotation(&moveList, 'a', 1, 'c', 2),
          "Cavalier a1 -> c2");


    /* --------------------------------------------------------
       Test 3 : pièce alliée
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'N';
    position.board[2][3] = 'P';

    GenerateKnightMoves(&position, &moveList);

    Check(!HasMoveChessNotation(&moveList, 'e', 4, 'd', 6),
          "Cavalier ne capture pas une pièce alliée");


    /* --------------------------------------------------------
       Test 4 : capture
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'N';
    position.board[2][3] = 'p';

    GenerateKnightMoves(&position, &moveList);

    Check(HasMoveChessNotation(&moveList, 'e', 4, 'd', 6),
          "Cavalier capture une pièce adverse");
}


/* ============================================================
   TESTS FOUS
   ============================================================ */

void TestBishops(void)
{
    Position position;
    MoveList moveList;

    PrintTestHeader("TESTS FOUS");


    /* --------------------------------------------------------
       Test 1 : fou au centre
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'B'; // e4

    GenerateBishopMoves(&position, &moveList);

    Check(moveList.count == 13,
          "Fou e4 : 13 coups");


    /* --------------------------------------------------------
       Test 2 : pièce alliée bloque
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'B';
    position.board[3][3] = 'P'; // d5

    GenerateBishopMoves(&position, &moveList);

    Check(!HasMoveChessNotation(&moveList, 'e', 4, 'c', 6),
          "Fou ne traverse pas une pièce alliée");


    /* --------------------------------------------------------
       Test 3 : capture puis arrêt
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'B';
    position.board[3][3] = 'p'; // d5
    position.board[2][2] = 'p'; // c6

    GenerateBishopMoves(&position, &moveList);

    Check(HasMoveChessNotation(&moveList, 'e', 4, 'd', 5),
          "Fou peut capturer la première pièce");

    Check(!HasMoveChessNotation(&moveList, 'e', 4, 'c', 6),
          "Fou ne traverse pas une pièce capturée");
}


/* ============================================================
   TESTS TOURS
   ============================================================ */

void TestRooks(void)
{
    Position position;
    MoveList moveList;

    PrintTestHeader("TESTS TOURS");


    /* --------------------------------------------------------
       Test 1 : tour au centre
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'R'; // e4

    GenerateRookMoves(&position, &moveList);

    Check(moveList.count == 14,
          "Tour e4 : 14 coups");


    /* --------------------------------------------------------
       Test 2 : blocage
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'R';
    position.board[4][6] = 'P'; // g4

    GenerateRookMoves(&position, &moveList);

    Check(!HasMoveChessNotation(&moveList, 'e', 4, 'h', 4),
          "Tour ne traverse pas une pièce alliée");


    /* --------------------------------------------------------
       Test 3 : capture
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'R';
    position.board[4][6] = 'p';

    GenerateRookMoves(&position, &moveList);

    Check(HasMoveChessNotation(&moveList, 'e', 4, 'g', 4),
          "Tour capture une pièce adverse");

    Check(!HasMoveChessNotation(&moveList, 'e', 4, 'h', 4),
          "Tour ne traverse pas la pièce capturée");
}


/* ============================================================
   TESTS DAME
   ============================================================ */

void TestQueens(void)
{
    Position position;
    MoveList moveList;

    PrintTestHeader("TESTS DAMES");


    /* --------------------------------------------------------
       Test 1 : dame au centre
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'Q'; // e4

    GenerateQueenMoves(&position, &moveList);

    Check(moveList.count == 27,
          "Dame e4 : 27 coups");


    /* --------------------------------------------------------
       Test 2 : blocage diagonal
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'Q';
    position.board[2][2] = 'P';

    GenerateQueenMoves(&position, &moveList);

    Check(!HasMoveChessNotation(&moveList, 'e', 4, 'b', 7),
          "Dame ne traverse pas une pièce en diagonale");


    /* --------------------------------------------------------
       Test 3 : blocage horizontal
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'Q';
    position.board[4][6] = 'P';

    GenerateQueenMoves(&position, &moveList);

    Check(!HasMoveChessNotation(&moveList, 'e', 4, 'h', 4),
          "Dame ne traverse pas une pièce horizontalement");


    /* --------------------------------------------------------
       Test 4 : capture
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'Q';
    position.board[4][6] = 'p';

    GenerateQueenMoves(&position, &moveList);

    Check(HasMoveChessNotation(&moveList, 'e', 4, 'g', 4),
          "Dame peut capturer");
}


/* ============================================================
   TESTS ROI
   ============================================================ */

void TestKings(void)
{
    Position position;
    MoveList moveList;

    PrintTestHeader("TESTS ROIS");


    /* --------------------------------------------------------
       Test 1 : roi au centre
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'K';

    GenerateKingMoves(&position, &moveList);

    Check(moveList.count == 8,
          "Roi e4 : 8 coups");


    /* --------------------------------------------------------
       Test 2 : roi dans un coin
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[7][0] = 'K';

    GenerateKingMoves(&position, &moveList);

    Check(moveList.count == 3,
          "Roi a1 : 3 coups");


    /* --------------------------------------------------------
       Test 3 : pièce alliée
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'K';
    position.board[3][3] = 'P';

    GenerateKingMoves(&position, &moveList);

    Check(!HasMoveChessNotation(&moveList, 'e', 4, 'd', 5),
          "Roi ne capture pas une pièce alliée");


    /* --------------------------------------------------------
       Test 4 : capture
       -------------------------------------------------------- */

    ResetEmptyPosition(&position);

    position.board[4][4] = 'K';
    position.board[3][3] = 'p';

    GenerateKingMoves(&position, &moveList);

    Check(HasMoveChessNotation(&moveList, 'e', 4, 'd', 5),
          "Roi peut capturer une pièce adverse");
}


/* ============================================================
   TESTS MAKE MOVE
   ============================================================ */

void TestMakeMove(void)
{
    Position position;

    PrintTestHeader("TESTS MAKE MOVE");

    InitBoard(&position);


    /* e2 -> e4 */

    Move move;

    move.fromRow = 6;
    move.fromCol = 4;

    move.toRow = 4;
    move.toCol = 4;

    MakeMove(&position, move);

    Check(position.board[4][4] == 'P',
          "MakeMove : pièce déplacée");

    Check(position.board[6][4] == '.',
          "MakeMove : case de départ vidée");

    Check(position.sideToMove == 1,
          "MakeMove : changement de joueur");


    /* e7 -> e5 */

    move.fromRow = 1;
    move.fromCol = 4;

    move.toRow = 3;
    move.toCol = 4;

    MakeMove(&position, move);

    Check(position.board[3][4] == 'p',
          "MakeMove : pion noir déplacé");

    Check(position.board[1][4] == '.',
          "MakeMove : ancienne case noire vidée");

    Check(position.sideToMove == 0,
          "MakeMove : retour aux blancs");
}


/* ============================================================
   TESTS CAPTURES
   ============================================================ */

void TestCaptures(void)
{
    Position position;

    PrintTestHeader("TESTS CAPTURES");

    ResetEmptyPosition(&position);

    position.board[4][4] = 'R';
    position.board[4][7] = 'p';

    position.sideToMove = 0;

    Move move;

    move.fromRow = 4;
    move.fromCol = 4;

    move.toRow = 4;
    move.toCol = 7;

    MakeMove(&position, move);

    Check(position.board[4][7] == 'R',
          "Capture : pièce adverse remplacée");

    Check(position.board[4][4] == '.',
          "Capture : case de départ vidée");
}


/* ============================================================
   TEST POSITION INITIALE
   ============================================================ */

void TestInitialPosition(void)
{
    Position position;

    PrintTestHeader("TEST POSITION INITIALE");

    InitBoard(&position);

    Check(position.board[7][0] == 'R',
          "Tour blanche a1");

    Check(position.board[7][4] == 'K',
          "Roi blanc e1");

    Check(position.board[0][4] == 'k',
          "Roi noir e8");

    Check(position.board[6][4] == 'P',
          "Pion blanc e2");

    Check(position.board[1][4] == 'p',
          "Pion noir e7");

    Check(position.sideToMove == 0,
          "Les blancs commencent");

    Check(position.whiteKingSideCastle == 1,
          "Roque blanc petit disponible");

    Check(position.whiteQueenSideCastle == 1,
          "Roque blanc grand disponible");

    Check(position.blackKingSideCastle == 1,
          "Roque noir petit disponible");

    Check(position.blackQueenSideCastle == 1,
          "Roque noir grand disponible");
}


/* ============================================================
   TESTS MULTIPLES PIECES
   ============================================================ */

void TestMultiplePieces(void)
{
    Position position;
    MoveList moveList;

    PrintTestHeader("TESTS PIECES MULTIPLES");

    ResetEmptyPosition(&position);

    position.board[4][4] = 'B';
    position.board[2][2] = 'B';

    GenerateBishopMoves(&position, &moveList);

    Check(moveList.count > 13,
          "Deux fous : génération des coups des deux pièces");


    ResetEmptyPosition(&position);

    position.board[4][4] = 'R';
    position.board[4][0] = 'R';

    GenerateRookMoves(&position, &moveList);

    Check(moveList.count > 14,
          "Deux tours : génération des coups des deux pièces");


    ResetEmptyPosition(&position);

    position.board[4][4] = 'N'; // e4
    position.board[7][7] = 'N'; // h1

    GenerateKnightMoves(&position, &moveList);

    printf("\nPosition testee :\n");
    PrintBoard(&position);

    printf("\nNombre total de coups : %d\n", moveList.count);
    printf("\nCoups disponibles :\n");

    PrintMoves(&moveList);

    Check(moveList.count == 10,
        "Deux cavaliers : generation correcte des coups");
}


/* ============================================================
   MAIN
   ============================================================ */

int main(void)
{
    printf("\n");
    printf("============================================================\n");
    printf("              CHESSBOT - TEST SUITE\n");
    printf("============================================================\n");

    TestInitialPosition();

    TestPawns();

    TestKnights();

    TestBishops();

    TestRooks();

    TestQueens();

    TestKings();

    TestMakeMove();

    TestCaptures();

    TestMultiplePieces();


    printf("\n");
    printf("============================================================\n");
    printf("                    RESULTATS\n");
    printf("============================================================\n");

    printf("\nTests reussis : %d\n", testsPassed);
    printf("Tests echoues : %d\n", testsFailed);
    printf("Total         : %d\n", testsPassed + testsFailed);

    if (testsFailed == 0)
    {
        printf("\n>>> TOUS LES TESTS PASSENT <<<\n");
        return 0;
    }
    else
    {
        printf("\n>>> DES TESTS ONT ECHOUE <<<\n");
        return 1;
    }
}