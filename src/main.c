#include <stdio.h>
#include "board.h"
#include "move.h"

void PrintMoves(MoveList *moveList)
{
    printf("Nombre de coups : %d\n", moveList->count);

    for (int i = 0; i < moveList->count; i++)
    {
        Move move = moveList->moves[i];

        printf("%d. %c%d -> %c%d\n",
               i + 1,
               'a' + move.fromCol,
               8 - move.fromRow,
               'a' + move.toCol,
               8 - move.toRow);
    }

    printf("\n");
}


int main(void)
{
    Position position;
    MoveList moveList;


    // =========================================================
    // TEST 1 : POSITION INITIALE
    // =========================================================

    printf("\n========== TEST 1 : POSITION INITIALE ==========\n\n");

    InitBoard(&position);

    PrintBoard(&position);

    GenerateKnightMoves(&position, &moveList);

    printf("Coups des cavaliers blancs :\n");
    PrintMoves(&moveList);


    // =========================================================
    // TEST 2 : CAVALIER BLANC AU CENTRE
    // =========================================================

    printf("\n========== TEST 2 : CAVALIER AU CENTRE ==========\n\n");

    InitBoard(&position);

    // On enlève toutes les pièces
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            position.board[i][j] = '.';
        }
    }

    // Cavalier blanc en e4
    position.board[4][4] = 'N';

    position.sideToMove = 0;

    PrintBoard(&position);

    GenerateKnightMoves(&position, &moveList);

    printf("Coups possibles depuis e4 :\n");
    PrintMoves(&moveList);


    // =========================================================
    // TEST 3 : CAVALIER CONTRE DES PIECES ALLIEES
    // =========================================================

    printf("\n========== TEST 3 : PIECES ALLIEES ==========\n\n");

    InitBoard(&position);

    // On vide le plateau
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            position.board[i][j] = '.';
        }
    }

    // Cavalier blanc en e4
    position.board[4][4] = 'N';

    // Pièces blanches autour
    position.board[2][3] = 'P'; // d6
    position.board[2][5] = 'P'; // f6
    position.board[3][2] = 'P'; // c5

    position.sideToMove = 0;

    PrintBoard(&position);

    GenerateKnightMoves(&position, &moveList);

    printf("Le cavalier ne doit PAS pouvoir capturer ses propres pieces :\n");
    PrintMoves(&moveList);


    // =========================================================
    // TEST 4 : CAPTURES
    // =========================================================

    printf("\n========== TEST 4 : CAPTURES ==========\n\n");

    InitBoard(&position);

    // On vide le plateau
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            position.board[i][j] = '.';
        }
    }

    // Cavalier blanc en e4
    position.board[4][4] = 'N';

    // Pièces noires capturables
    position.board[2][3] = 'p'; // d6
    position.board[2][5] = 'p'; // f6
    position.board[3][2] = 'p'; // c5
    position.board[3][6] = 'p'; // g5
    position.board[5][2] = 'p'; // c3
    position.board[5][6] = 'p'; // g3
    position.board[6][3] = 'p'; // d2
    position.board[6][5] = 'p'; // f2

    position.sideToMove = 0;

    PrintBoard(&position);

    GenerateKnightMoves(&position, &moveList);

    printf("Le cavalier doit pouvoir capturer les 8 pieces noires :\n");
    PrintMoves(&moveList);


    // =========================================================
    // TEST 5 : CAVALIER NOIR
    // =========================================================

    printf("\n========== TEST 5 : CAVALIER NOIR ==========\n\n");

    InitBoard(&position);

    // On vide le plateau
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            position.board[i][j] = '.';
        }
    }

    // Cavalier noir en e5
    position.board[3][4] = 'n';

    position.sideToMove = 1;

    PrintBoard(&position);

    GenerateKnightMoves(&position, &moveList);

    printf("Coups possibles du cavalier noir depuis e5 :\n");
    PrintMoves(&moveList);


    // =========================================================
    // TEST 6 : CAVALIER DANS UN COIN
    // =========================================================

    printf("\n========== TEST 6 : CAVALIER DANS UN COIN ==========\n\n");

    InitBoard(&position);

    // On vide le plateau
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            position.board[i][j] = '.';
        }
    }

    // Cavalier blanc en a1
    position.board[7][0] = 'N';

    position.sideToMove = 0;

    PrintBoard(&position);

    GenerateKnightMoves(&position, &moveList);

    printf("Depuis a1, le cavalier ne doit avoir que 2 coups :\n");
    PrintMoves(&moveList);


    printf("\n========== FIN DES TESTS ==========\n");

    return 0;
}