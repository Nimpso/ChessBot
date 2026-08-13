#include <stdio.h>
#include "board.h"

//////////////////////////////////////////////////////////////////////////////////
// les blancs (0) sont en MAJ les noirs (1) sont en min //
//////////////////////////////////////////////////////////////////////////////////

void InitBoard(Position *position)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            position->board[i][j] = '.';
        }
    }

    position->board[0][0] = 'r';
    position->board[0][1] = 'n';
    position->board[0][2] = 'b';
    position->board[0][3] = 'q';
    position->board[0][4] = 'k';
    position->board[0][5] = 'b';
    position->board[0][6] = 'n';
    position->board[0][7] = 'r';

    for (int j = 0; j < 8; j++)
    {
        position->board[1][j] = 'p';
        position->board[6][j] = 'P';
    }

    position->board[7][0] = 'R';
    position->board[7][1] = 'N';
    position->board[7][2] = 'B';
    position->board[7][3] = 'Q';
    position->board[7][4] = 'K';
    position->board[7][5] = 'B';
    position->board[7][6] = 'N';
    position->board[7][7] = 'R';

    position->sideToMove = 0;

    position->whiteKingSideCastle = 1;
    position->whiteQueenSideCastle = 1;
    position->blackKingSideCastle = 1;
    position->blackQueenSideCastle = 1;

    position->enPassantRow = -1;
    position->enPassantCol = -1;

    position->halfMoveClock = 0;
}

void PrintBoard(Position *position)
{
    for (int i = 0; i < 8; i++)
    {
        printf("%d ", 8 - i);

        for (int j = 0; j < 8; j++)
        {
            printf("%c ", position->board[i][j]);
        }

        printf("\n");
    }

    printf("  a b c d e f g h\n");
}

int IsWhitePiece(char piece)
{
    return piece >= 'A' && piece <= 'Z';
}

int IsBlackPiece(char piece)
{
    return piece >= 'a' && piece <= 'z';
}