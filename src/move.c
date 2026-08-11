#include <stdio.h>
#include "move.h"

void MakeMove(Position *position, Move move)
{
    position->board[move.toRow][move.toCol] = position->board[move.fromRow][move.fromCol]; //case maintenant = case avant
    position->board[move.fromRow][move.fromCol] = '.'; //case avant = '.'
    position->sideToMove = (position->sideToMove == 0) ? 1 : 0;

    printf("%c%d -> %c%d\n",
    'a' + move.fromCol,
    8 - move.fromRow,
    'a' + move.toCol,
    8 - move.toRow);
}

void GeneratePawnMoves(Position *position, MoveList *moveList)
{
    moveList->count = 0;
    if(position->sideToMove == 0) // Blancs
    {
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if(position->board[i][j] == 'P')
                {
                     if (i > 0 && position->board[i - 1][j] == '.')  // 1 case
                    {
                        moveList->moves[moveList->count].fromRow = i;
                        moveList->moves[moveList->count].fromCol = j;
                        moveList->moves[moveList->count].toRow = i - 1;
                        moveList->moves[moveList->count].toCol = j;
                        moveList->count++;
                        if (i == 6 && position->board[i - 2][j] == '.') // 2 cases
                        {
                            moveList->moves[moveList->count].fromRow = i;
                            moveList->moves[moveList->count].fromCol = j;
                            moveList->moves[moveList->count].toRow = i - 2;
                            moveList->moves[moveList->count].toCol = j;
                            moveList->count++;
                        }
                    }
                }
        
            }
        }
    }
    else  //Noirs
    {
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if(position->board[i][j] == 'p')
                {
                    if (i < 7 && position->board[i + 1][j] == '.') //1 case
                    {
                        moveList->moves[moveList->count].fromRow = i;
                        moveList->moves[moveList->count].fromCol = j;
                        moveList->moves[moveList->count].toRow = i + 1;
                        moveList->moves[moveList->count].toCol = j;
                        moveList->count++;
                        
                        if (i == 1 && position->board[i + 2][j] == '.') //2 cases
                        {
                            moveList->moves[moveList->count].fromRow = i;
                            moveList->moves[moveList->count].fromCol = j;
                            moveList->moves[moveList->count].toRow = i + 2;
                            moveList->moves[moveList->count].toCol = j;
                            moveList->count++;
                        }
                    }
                    
                }
        
            }
        }
    }
}