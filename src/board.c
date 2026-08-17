#include <stdio.h>
#include <string.h>
#include <ctype.h>
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

int ParseFEN(Position *position, const char *fen)
{
    // Initialiser le plateau vide
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            position->board[i][j] = '.';
        }
    }

    // Reset des droits
    position->whiteKingSideCastle = 0;
    position->whiteQueenSideCastle = 0;
    position->blackKingSideCastle = 0;
    position->blackQueenSideCastle = 0;
    position->enPassantRow = -1;
    position->enPassantCol = -1;
    position->halfMoveClock = 0;
    position->sideToMove = 0;

    int row = 0, col = 0;
    const char *p = fen;

    // 1. Placement des pièces
    while (*p && *p != ' ')
    {
        if (*p == '/')
        {
            row++;
            col = 0;
        }
        else if (isdigit(*p))
        {
            int empty = *p - '0';
            for (int i = 0; i < empty; i++)
            {
                if (col < 8)
                {
                    position->board[row][col] = '.';
                    col++;
                }
            }
        }
        else
        {
            // Pièce (lettre majuscule = blanc, minuscule = noir)
            if (col < 8 && row < 8)
            {
                position->board[row][col] = *p;
                col++;
            }
        }
        p++;
    }

    // 2. Trait (w = blancs, b = noirs)
    if (*p == ' ')
    {
        p++;
        if (*p == 'w')
            position->sideToMove = 0;
        else if (*p == 'b')
            position->sideToMove = 1;
        p++;
    }

    // 3. Droits de roque
    if (*p == ' ')
    {
        p++;
        if (*p == '-')
        {
            // Pas de droits de roque
            p++;
        }
        else
        {
            while (*p && *p != ' ')
            {
                if (*p == 'K') position->whiteKingSideCastle = 1;
                if (*p == 'Q') position->whiteQueenSideCastle = 1;
                if (*p == 'k') position->blackKingSideCastle = 1;
                if (*p == 'q') position->blackQueenSideCastle = 1;
                p++;
            }
        }
    }

    // 4. En passant
    if (*p == ' ')
    {
        p++;
        if (*p == '-')
        {
            p++;
        }
        else
        {
            if (isalpha(*p))
            {
                int colEP = *p - 'a';
                p++;
                if (isdigit(*p))
                {
                    int rowEP = 8 - (*p - '0');
                    position->enPassantRow = rowEP;
                    position->enPassantCol = colEP;
                }
            }
            p++;
        }
    }

    // 5. Halfmove clock (optionnel)
    if (*p == ' ')
    {
        p++;
        if (isdigit(*p))
        {
            int clock = 0;
            while (isdigit(*p))
            {
                clock = clock * 10 + (*p - '0');
                p++;
            }
            position->halfMoveClock = clock;
        }
    }

    return 1; // Succès
}

void InitPositionFromFEN(Position *position, const char *fen)
{
    ParseFEN(position, fen);
}