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
                    // manger à droite
                    if (i > 0 && j < 7 && IsBlackPiece(position->board[i - 1][j + 1]))
                    {
                        moveList->moves[moveList->count].fromRow = i;
                        moveList->moves[moveList->count].fromCol = j;
                        moveList->moves[moveList->count].toRow = i - 1;
                        moveList->moves[moveList->count].toCol = j + 1;
                        moveList->count++;
                    }

                    // manger à gauche
                    if (i > 0 && j > 0 && IsBlackPiece(position->board[i - 1][j - 1]))
                    {
                        moveList->moves[moveList->count].fromRow = i;
                        moveList->moves[moveList->count].fromCol = j;
                        moveList->moves[moveList->count].toRow = i - 1;
                        moveList->moves[moveList->count].toCol = j - 1;
                        moveList->count++;
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
                    // manger à gauche
                    if (i < 7 && j < 7 && IsWhitePiece(position->board[i + 1][j + 1]))
                    {
                        moveList->moves[moveList->count].fromRow = i;
                        moveList->moves[moveList->count].fromCol = j;
                        moveList->moves[moveList->count].toRow = i + 1;
                        moveList->moves[moveList->count].toCol = j + 1;
                        moveList->count++;
                    }

                    // manger à droite
                    if (i < 7 && j > 0 && IsWhitePiece(position->board[i + 1][j - 1]))
                    {
                        moveList->moves[moveList->count].fromRow = i;
                        moveList->moves[moveList->count].fromCol = j;
                        moveList->moves[moveList->count].toRow = i + 1;
                        moveList->moves[moveList->count].toCol = j - 1;
                        moveList->count++;
                    }
                    
                }
        
            }
        }
    }
}

void GenerateKnightMoves(Position *position, MoveList *moveList)
{
    int knightMoves[8][2] =
    {
        {-2, -1},
        {-2, +1},
        {-1, -2},
        {-1, +2},
        {+1, -2},
        {+1, +2},
        {+2, -1},
        {+2, +1}
    };

    moveList->count = 0;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            char piece = position->board[i][j];

            // On cherche uniquement les cavaliers du joueur actuel
            if (position->sideToMove == 0 && piece != 'N')
                continue;

            if (position->sideToMove == 1 && piece != 'n')
                continue;

            // Tester les 8 déplacements
            for (int k = 0; k < 8; k++)
            {
                int newRow = i + knightMoves[k][0];
                int newCol = j + knightMoves[k][1];

                // Hors de l'échiquier
                if (newRow < 0 || newRow > 7 ||
                    newCol < 0 || newCol > 7)
                {
                    continue;
                }

                char target = position->board[newRow][newCol];

                // Case occupée par une pièce alliée
                if (position->sideToMove == 0 && IsWhitePiece(target))
                    continue;

                if (position->sideToMove == 1 && IsBlackPiece(target))
                    continue;

                // Le déplacement est valide
                moveList->moves[moveList->count].fromRow = i;
                moveList->moves[moveList->count].fromCol = j;
                moveList->moves[moveList->count].toRow = newRow;
                moveList->moves[moveList->count].toCol = newCol;

                moveList->count++;
            }
        }
    }
}