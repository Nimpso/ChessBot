#include <stdio.h>
#include "board.h"
#include "move.h"


void PrintMoveList(MoveList *moveList)
{
    printf("Nombre de coups : %d\n", moveList->count);

    for (int i = 0; i < moveList->count; i++)
    {
        Move move = moveList->moves[i];

        printf("  %2d. %c%d -> %c%d\n",
               i + 1,
               'a' + move.fromCol,
               8 - move.fromRow,
               'a' + move.toCol,
               8 - move.toRow);
    }
}


void ResetPosition(Position *position)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            position->board[i][j] = '.';
        }
    }
}


void TestPosition(Position *position, MoveList *moveList)
{
    PrintBoard(position);

    printf("Side : %s\n",
           position->sideToMove == 0 ? "White" : "Black");

    GeneratePawnMoves(position, moveList);
    PrintMoveList(moveList);

    printf("\n");
}


int main(void)
{
    Position position;
    MoveList moveList;


    printf("\n");
    printf("========================================\n");
    printf("        CHESSBOT - PAWN TESTS\n");
    printf("========================================\n");


    /*
     * TEST 1
     * Position initiale
     *
     * Blancs :
     * 8 pions x 2 coups = 16 coups
     */
    printf("\n========================================\n");
    printf("TEST 1 : POSITION INITIALE - BLANCS\n");
    printf("========================================\n");

    InitBoard(&position);

    TestPosition(&position, &moveList);


    /*
     * TEST 2
     * Position initiale - Noirs
     *
     * On change simplement le joueur.
     * On doit également avoir 16 coups.
     */
    printf("\n========================================\n");
    printf("TEST 2 : POSITION INITIALE - NOIRS\n");
    printf("========================================\n");

    position.sideToMove = 1;

    TestPosition(&position, &moveList);


    /*
     * TEST 3
     * Pion blanc seul au milieu.
     *
     * c4 -> c5
     *
     * Un seul coup.
     */
    printf("\n========================================\n");
    printf("TEST 3 : PION BLANC - AVANCER\n");
    printf("========================================\n");

    ResetPosition(&position);

    position.sideToMove = 0;

    position.board[4][2] = 'P'; // c4

    TestPosition(&position, &moveList);


    /*
     * TEST 4
     * Pion blanc sur sa ligne de départ.
     *
     * c2 -> c3
     * c2 -> c4
     *
     * Deux coups.
     */
    printf("\n========================================\n");
    printf("TEST 4 : PION BLANC - DOUBLE PAS\n");
    printf("========================================\n");

    ResetPosition(&position);

    position.sideToMove = 0;

    position.board[6][2] = 'P'; // c2

    TestPosition(&position, &moveList);


    /*
     * TEST 5
     * Pion blanc avec deux captures possibles.
     *
     *       p
     *       /
     *      P
     *       \
     *       p
     *
     * c4 -> c5
     * c4 -> b5
     * c4 -> d5
     *
     * Trois coups.
     */
    printf("\n========================================\n");
    printf("TEST 5 : PION BLANC - DEUX CAPTURES\n");
    printf("========================================\n");

    ResetPosition(&position);

    position.sideToMove = 0;

    position.board[4][2] = 'P'; // c4
    position.board[3][1] = 'p'; // b5
    position.board[3][3] = 'p'; // d5

    TestPosition(&position, &moveList);


    /*
     * TEST 6
     * Pion blanc avec uniquement une capture
     *
     * c4 -> d5
     *
     * b5 est vide.
     */
    printf("\n========================================\n");
    printf("TEST 6 : PION BLANC - UNE CAPTURE\n");
    printf("========================================\n");

    ResetPosition(&position);

    position.sideToMove = 0;

    position.board[4][2] = 'P'; // c4
    position.board[3][3] = 'p'; // d5

    TestPosition(&position, &moveList);


    /*
     * TEST 7
     * Pion blanc bloqué.
     *
     * c4 -> impossible
     *
     * Aucun coup.
     */
    printf("\n========================================\n");
    printf("TEST 7 : PION BLANC - BLOQUE\n");
    printf("========================================\n");

    ResetPosition(&position);

    position.sideToMove = 0;

    position.board[4][2] = 'P'; // c4
    position.board[3][2] = 'p'; // c5

    TestPosition(&position, &moveList);


    /*
     * TEST 8
     * Pion blanc sur la colonne A.
     *
     * Permet de vérifier qu'on ne fait pas :
     *
     * j - 1 = -1
     *
     * Le programme doit simplement générer :
     *
     * a4 -> a5
     * a4 -> b5
     */
    printf("\n========================================\n");
    printf("TEST 8 : PION BLANC - BORD GAUCHE\n");
    printf("========================================\n");

    ResetPosition(&position);

    position.sideToMove = 0;

    position.board[4][0] = 'P'; // a4
    position.board[3][1] = 'p'; // b5

    TestPosition(&position, &moveList);


    /*
     * TEST 9
     * Pion blanc sur la colonne H.
     *
     * Vérifie qu'on ne fait pas :
     *
     * j + 1 = 8
     *
     * On doit avoir :
     *
     * h4 -> h5
     * h4 -> g5
     */
    printf("\n========================================\n");
    printf("TEST 9 : PION BLANC - BORD DROIT\n");
    printf("========================================\n");

    ResetPosition(&position);

    position.sideToMove = 0;

    position.board[4][7] = 'P'; // h4
    position.board[3][6] = 'p'; // g5

    TestPosition(&position, &moveList);


    /*
     * TEST 10
     * Pion noir seul au milieu.
     *
     * c5 -> c4
     */
    printf("\n========================================\n");
    printf("TEST 10 : PION NOIR - AVANCER\n");
    printf("========================================\n");

    ResetPosition(&position);

    position.sideToMove = 1;

    position.board[3][2] = 'p'; // c5

    TestPosition(&position, &moveList);


    /*
     * TEST 11
     * Pion noir sur sa ligne de départ.
     *
     * c7 -> c6
     * c7 -> c5
     */
    printf("\n========================================\n");
    printf("TEST 11 : PION NOIR - DOUBLE PAS\n");
    printf("========================================\n");

    ResetPosition(&position);

    position.sideToMove = 1;

    position.board[1][2] = 'p'; // c7

    TestPosition(&position, &moveList);


    /*
     * TEST 12
     * Pion noir avec deux captures.
     *
     * c5 -> c4
     * c5 -> b4
     * c5 -> d4
     */
    printf("\n========================================\n");
    printf("TEST 12 : PION NOIR - DEUX CAPTURES\n");
    printf("========================================\n");

    ResetPosition(&position);

    position.sideToMove = 1;

    position.board[3][2] = 'p'; // c5
    position.board[4][1] = 'P'; // b4
    position.board[4][3] = 'P'; // d4

    TestPosition(&position, &moveList);


    /*
     * TEST 13
     * Pion noir avec une seule capture.
     */
    printf("\n========================================\n");
    printf("TEST 13 : PION NOIR - UNE CAPTURE\n");
    printf("========================================\n");

    ResetPosition(&position);

    position.sideToMove = 1;

    position.board[3][2] = 'p'; // c5
    position.board[4][3] = 'P'; // d4

    TestPosition(&position, &moveList);


    /*
     * TEST 14
     * Pion noir bloqué.
     */
    printf("\n========================================\n");
    printf("TEST 14 : PION NOIR - BLOQUE\n");
    printf("========================================\n");

    ResetPosition(&position);

    position.sideToMove = 1;

    position.board[3][2] = 'p'; // c5
    position.board[4][2] = 'P'; // c4

    TestPosition(&position, &moveList);


    /*
     * TEST 15
     * Pion noir sur le bord gauche.
     */
    printf("\n========================================\n");
    printf("TEST 15 : PION NOIR - BORD GAUCHE\n");
    printf("========================================\n");

    ResetPosition(&position);

    position.sideToMove = 1;

    position.board[3][0] = 'p'; // a5
    position.board[4][1] = 'P'; // b4

    TestPosition(&position, &moveList);


    /*
     * TEST 16
     * Pion noir sur le bord droit.
     */
    printf("\n========================================\n");
    printf("TEST 16 : PION NOIR - BORD DROIT\n");
    printf("========================================\n");

    ResetPosition(&position);

    position.sideToMove = 1;

    position.board[3][7] = 'p'; // h5
    position.board[4][6] = 'P'; // g4

    TestPosition(&position, &moveList);


    /*
     * TEST 17
     * Vérification du blocage du double mouvement blanc.
     *
     * c2 -> c3 est impossible
     * donc c2 -> c4 doit également être impossible.
     *
     * Aucun coup.
     */
    printf("\n========================================\n");
    printf("TEST 17 : DOUBLE PAS BLANC BLOQUE\n");
    printf("========================================\n");

    ResetPosition(&position);

    position.sideToMove = 0;

    position.board[6][2] = 'P'; // c2
    position.board[5][2] = 'p'; // c3

    TestPosition(&position, &moveList);


    /*
     * TEST 18
     * Vérification du blocage du double mouvement noir.
     *
     * c7 -> c6 est impossible
     * donc c7 -> c5 doit également être impossible.
     */
    printf("\n========================================\n");
    printf("TEST 18 : DOUBLE PAS NOIR BLOQUE\n");
    printf("========================================\n");

    ResetPosition(&position);

    position.sideToMove = 1;

    position.board[1][2] = 'p'; // c7
    position.board[2][2] = 'P'; // c6

    TestPosition(&position, &moveList);


    printf("\n========================================\n");
    printf("             TESTS TERMINES\n");
    printf("========================================\n");

    return 0;
}
