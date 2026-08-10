// les blancs sont en majuscules et noirs en minuscule 
#include <stdio.h>
#include "board.h"

char board [8][8];

void InitBoard(void)
{
    for (int i = 0; i<8; i++)
    {
        for (int j = 0; j<8; j++)
        {
            board [i][j] = '.';
        }
    }

    board [0][0] = 'r';
    board [0][1] = 'n';
    board [0][2] = 'b';
    board [0][3] = 'q';
    board [0][4] = 'k';
    board [0][5] = 'b';
    board [0][6] = 'n';
    board [0][7] = 'r';

    for (int j = 0; j<8; j++)
        {
            board [1][j] = 'p';
            board [6][j] = 'P';
        }
    
    board [7][0] = 'R';
    board [7][1] = 'N';
    board [7][2] = 'B';
    board [7][3] = 'Q';
    board [7][4] = 'K';
    board [7][5] = 'B';
    board [7][6] = 'N';
    board [7][7] = 'R';
}

void PrintBoard(void)
{
    for (int i = 0; i<8; i++)
    {
        printf("%d ", (8-i));

        for (int j = 0; j<8; j++)
        {
            printf("%c ", board[i][j]);
        }

        printf("\n");
    }
    printf("  a b c d e f g h\n");
}
