#include <stdio.h>
#include "move.h"

void AddMove(MoveList *moveList, int fromRow, int fromCol, int toRow, int toCol)
{
    moveList->moves[moveList->count].fromRow = fromRow;
    moveList->moves[moveList->count].fromCol = fromCol;
    moveList->moves[moveList->count].toRow = toRow;
    moveList->moves[moveList->count].toCol = toCol;
    moveList->moves[moveList->count].promotion = '\0';
    moveList->moves[moveList->count].enPassant = 0;

    moveList->count++;
}

void AddPromotionMove(MoveList *moveList, int fromRow, int fromCol, int toRow, int toCol, char promotion)
{
    moveList->moves[moveList->count].fromRow = fromRow;
    moveList->moves[moveList->count].fromCol = fromCol;
    moveList->moves[moveList->count].toRow = toRow;
    moveList->moves[moveList->count].toCol = toCol;
    moveList->moves[moveList->count].promotion = promotion;
    moveList->moves[moveList->count].enPassant = 0;

    moveList->count++;
}

void AddEnPassantMove(MoveList *moveList, int fromRow, int fromCol, int toRow, int toCol)
{
    moveList->moves[moveList->count].fromRow = fromRow;
    moveList->moves[moveList->count].fromCol = fromCol;
    moveList->moves[moveList->count].toRow = toRow;
    moveList->moves[moveList->count].toCol = toCol;
    moveList->moves[moveList->count].promotion = '\0';
    moveList->moves[moveList->count].enPassant = 1;

    moveList->count++;
}

void MakeMove(Position *position, Move move)
{
    char movingPiece = position->board[move.fromRow][move.fromCol];

    /*
     * Capture en passant : la pièce prise n'est pas sur la case
     * d'arrivée mais sur la même rangée que le pion qui capture.
     */
    if (move.enPassant)
    {
        position->board[move.fromRow][move.toCol] = '.';
    }

    if (move.promotion != '\0')
    {
        position->board[move.toRow][move.toCol] = move.promotion;
    }
    else
    {
        position->board[move.toRow][move.toCol] = movingPiece; //case maintenant = case avant
    }

    position->board[move.fromRow][move.fromCol] = '.'; //case avant = '.'

    /*
     * Roque : le roi vient de bouger de 2 cases, il faut
     * déplacer la tour concernée en miroir.
     */
    if (movingPiece == 'K' && move.fromCol == 4 && move.toCol == 6) // petit roque blanc
    {
        position->board[7][5] = position->board[7][7];
        position->board[7][7] = '.';
    }
    else if (movingPiece == 'K' && move.fromCol == 4 && move.toCol == 2) // grand roque blanc
    {
        position->board[7][3] = position->board[7][0];
        position->board[7][0] = '.';
    }
    else if (movingPiece == 'k' && move.fromCol == 4 && move.toCol == 6) // petit roque noir
    {
        position->board[0][5] = position->board[0][7];
        position->board[0][7] = '.';
    }
    else if (movingPiece == 'k' && move.fromCol == 4 && move.toCol == 2) // grand roque noir
    {
        position->board[0][3] = position->board[0][0];
        position->board[0][0] = '.';
    }

    /*
     * Mise à jour des droits de roque.
     * Le roi qui bouge perd ses deux droits.
     */
    if (movingPiece == 'K')
    {
        position->whiteKingSideCastle = 0;
        position->whiteQueenSideCastle = 0;
    }
    else if (movingPiece == 'k')
    {
        position->blackKingSideCastle = 0;
        position->blackQueenSideCastle = 0;
    }

    /*
     * Une tour qui bouge, ou qui se fait capturer,
     * perd son droit de roque associé.
     */
    if (move.fromRow == 7 && move.fromCol == 0) position->whiteQueenSideCastle = 0;
    if (move.fromRow == 7 && move.fromCol == 7) position->whiteKingSideCastle  = 0;
    if (move.fromRow == 0 && move.fromCol == 0) position->blackQueenSideCastle = 0;
    if (move.fromRow == 0 && move.fromCol == 7) position->blackKingSideCastle  = 0;

    if (move.toRow == 7 && move.toCol == 0) position->whiteQueenSideCastle = 0;
    if (move.toRow == 7 && move.toCol == 7) position->whiteKingSideCastle  = 0;
    if (move.toRow == 0 && move.toCol == 0) position->blackQueenSideCastle = 0;
    if (move.toRow == 0 && move.toCol == 7) position->blackKingSideCastle  = 0;

    /*
     * La case en passant ne vit qu'un seul demi-coup :
     * on la réinitialise puis on la recalcule si ce coup
     * est une avance de pion de 2 cases.
     */
    position->enPassantRow = -1;
    position->enPassantCol = -1;

    if ((movingPiece == 'P' || movingPiece == 'p') &&
        (move.toRow - move.fromRow == 2 || move.fromRow - move.toRow == 2))
    {
        position->enPassantRow = (move.fromRow + move.toRow) / 2;
        position->enPassantCol = move.fromCol;
    }

    position->sideToMove = (position->sideToMove == 0) ? 1 : 0;

    /*printf("%c%d -> %c%d\n",
    'a' + move.fromCol,
    8 - move.fromRow,
    'a' + move.toCol,
    8 - move.toRow);*/
}

void GeneratePawnMoves(Position *position, MoveList *moveList)
{
    
    if(position->sideToMove == 0) // Blancs
    {
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if(position->board[i][j] == 'P')
                {
                    // La rangée 0 (rang 8) est la rangée de promotion pour les blancs
                    int isPromotionRank = (i - 1 == 0);

                     if (i > 0 && position->board[i - 1][j] == '.')  // 1 case
                    {
                        if (isPromotionRank)
                        {
                            AddPromotionMove(moveList, i, j, i - 1, j, 'Q');
                            AddPromotionMove(moveList, i, j, i - 1, j, 'R');
                            AddPromotionMove(moveList, i, j, i - 1, j, 'B');
                            AddPromotionMove(moveList, i, j, i - 1, j, 'N');
                        }
                        else
                        {
                            AddMove(moveList, i, j, i - 1, j);
                        }

                        if (i == 6 && position->board[i - 2][j] == '.') // 2 cases (jamais de promotion)
                        {
                            AddMove(moveList, i, j, i - 2, j);
                        }
                    }
                    // manger à droite
                    if (i > 0 && j < 7 && IsBlackPiece(position->board[i - 1][j + 1]))
                    {
                        if (isPromotionRank)
                        {
                            AddPromotionMove(moveList, i, j, i - 1, j + 1, 'Q');
                            AddPromotionMove(moveList, i, j, i - 1, j + 1, 'R');
                            AddPromotionMove(moveList, i, j, i - 1, j + 1, 'B');
                            AddPromotionMove(moveList, i, j, i - 1, j + 1, 'N');
                        }
                        else
                        {
                            AddMove(moveList, i, j, i - 1, j+1);
                        }
                    }

                    // manger à gauche
                    if (i > 0 && j > 0 && IsBlackPiece(position->board[i - 1][j - 1]))
                    {
                        if (isPromotionRank)
                        {
                            AddPromotionMove(moveList, i, j, i - 1, j - 1, 'Q');
                            AddPromotionMove(moveList, i, j, i - 1, j - 1, 'R');
                            AddPromotionMove(moveList, i, j, i - 1, j - 1, 'B');
                            AddPromotionMove(moveList, i, j, i - 1, j - 1, 'N');
                        }
                        else
                        {
                            AddMove(moveList, i, j, i - 1, j-1);
                        }
                    }

                    // prise en passant
                    if (i > 0 && position->enPassantRow == i - 1 &&
                        (position->enPassantCol == j - 1 || position->enPassantCol == j + 1))
                    {
                        AddEnPassantMove(moveList, i, j, i - 1, position->enPassantCol);
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
                    // La rangée 7 (rang 1) est la rangée de promotion pour les noirs
                    int isPromotionRank = (i + 1 == 7);

                    if (i < 7 && position->board[i + 1][j] == '.') //1 case
                    {
                        if (isPromotionRank)
                        {
                            AddPromotionMove(moveList, i, j, i + 1, j, 'q');
                            AddPromotionMove(moveList, i, j, i + 1, j, 'r');
                            AddPromotionMove(moveList, i, j, i + 1, j, 'b');
                            AddPromotionMove(moveList, i, j, i + 1, j, 'n');
                        }
                        else
                        {
                            AddMove(moveList, i, j, i + 1 , j);
                        }
                        
                        if (i == 1 && position->board[i + 2][j] == '.') //2 cases (jamais de promotion)
                        {
                            AddMove(moveList, i, j, i + 2, j);
                        }
                    }
                    // manger à gauche
                    if (i < 7 && j < 7 && IsWhitePiece(position->board[i + 1][j + 1]))
                    {
                        if (isPromotionRank)
                        {
                            AddPromotionMove(moveList, i, j, i + 1, j + 1, 'q');
                            AddPromotionMove(moveList, i, j, i + 1, j + 1, 'r');
                            AddPromotionMove(moveList, i, j, i + 1, j + 1, 'b');
                            AddPromotionMove(moveList, i, j, i + 1, j + 1, 'n');
                        }
                        else
                        {
                            AddMove(moveList, i, j, i + 1, j + 1);
                        }
                    }

                    // manger à droite
                    if (i < 7 && j > 0 && IsWhitePiece(position->board[i + 1][j - 1]))
                    {
                        if (isPromotionRank)
                        {
                            AddPromotionMove(moveList, i, j, i + 1, j - 1, 'q');
                            AddPromotionMove(moveList, i, j, i + 1, j - 1, 'r');
                            AddPromotionMove(moveList, i, j, i + 1, j - 1, 'b');
                            AddPromotionMove(moveList, i, j, i + 1, j - 1, 'n');
                        }
                        else
                        {
                            AddMove(moveList, i, j, i + 1, j - 1);
                        }
                    }

                    // prise en passant
                    if (i < 7 && position->enPassantRow == i + 1 &&
                        (position->enPassantCol == j - 1 || position->enPassantCol == j + 1))
                    {
                        AddEnPassantMove(moveList, i, j, i + 1, position->enPassantCol);
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
                AddMove(moveList, i, j, newRow, newCol);
            }
        }
    }
}

void GenerateBishopMoves(Position *position, MoveList *moveList)
{
    int bishopDirections[4][2] =
    {
        {-1, -1},
        {-1, +1},
        {+1, -1},
        {+1, +1}
    };


    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            char piece = position->board[i][j];

            // On cherche uniquement les fous du joueur actuel
            if (position->sideToMove == 0 && piece != 'B')
                continue;

            if (position->sideToMove == 1 && piece != 'b')
                continue;

            // Parcours des 4 diagonales
            for (int direction = 0; direction < 4; direction++)
            {
                int newRow = i + bishopDirections[direction][0];
                int newCol = j + bishopDirections[direction][1];

                while (newRow >= 0 && newRow < 8 &&
                       newCol >= 0 && newCol < 8)
                {
                    char target = position->board[newRow][newCol];

                    // Case vide : déplacement possible
                    if (target == '.')
                    {
                        AddMove(moveList, i, j, newRow, newCol);
                    }
                    else
                    {
                        // piece bonne couleur stop
                        if (position->sideToMove == 0 && IsWhitePiece(target))
                            break;

                        if (position->sideToMove == 1 && IsBlackPiece(target))
                            break;

                        // mange adversaire
                        AddMove(moveList, i, j, newRow, newCol);

                        // stop apres rencontre
                        break;
                    }

                    newRow += bishopDirections[direction][0];
                    newCol += bishopDirections[direction][1];
                }
            }
        }
    }
}

void GenerateRookMoves(Position *position, MoveList *moveList)
{
    int rookDirections[4][2] =
    {
        {-1, 0},
        {+1, 0},
        {0, -1},
        {0, +1}
    };


    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            char piece = position->board[i][j];

            if (position->sideToMove == 0 && piece != 'R')
                continue;

            if (position->sideToMove == 1 && piece != 'r')
                continue;

            for (int direction = 0; direction < 4; direction++)
            {
                int newRow = i + rookDirections[direction][0];
                int newCol = j + rookDirections[direction][1];

                while (newRow >= 0 && newRow < 8 &&
                       newCol >= 0 && newCol < 8)
                {
                    char target = position->board[newRow][newCol];

                    if (target == '.')
                    {
                        AddMove(moveList, i, j, newRow, newCol);
                    }
                    else
                    {
                        if (position->sideToMove == 0 && IsWhitePiece(target))
                            break;

                        if (position->sideToMove == 1 && IsBlackPiece(target))
                            break;

                        // Capture
                        AddMove(moveList, i, j, newRow, newCol);
                        break;
                    }

                    newRow += rookDirections[direction][0];
                    newCol += rookDirections[direction][1];
                }
            }
        }
    }
}

void GenerateQueenMoves(Position *position, MoveList *moveList)
{
    int queenDirections[8][2] =
    {
        {-1, -1},
        {-1, 0},
        {-1, +1},
        {0, -1},
        {0, +1},
        {+1, -1},
        {+1, 0},
        {+1, +1}
    };


    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            char piece = position->board[i][j];

            if (position->sideToMove == 0 && piece != 'Q')
                continue;

            if (position->sideToMove == 1 && piece != 'q')
                continue;

            for (int direction = 0; direction < 8; direction++)
            {
                int newRow = i + queenDirections[direction][0];
                int newCol = j + queenDirections[direction][1];

                while (newRow >= 0 && newRow < 8 &&
                       newCol >= 0 && newCol < 8)
                {
                    char target = position->board[newRow][newCol];

                    if (target == '.')
                    {
                        AddMove(moveList, i, j, newRow, newCol);
                    }
                    else
                    {
                        if (position->sideToMove == 0 && IsWhitePiece(target))
                            break;

                        if (position->sideToMove == 1 && IsBlackPiece(target))
                            break;

                        // Capture
                        AddMove(moveList, i, j, newRow, newCol);

                        break;
                    }

                    newRow += queenDirections[direction][0];
                    newCol += queenDirections[direction][1];
                }
            }
        }
    }
}

void GenerateKingMoves(Position *position, MoveList *moveList)
{
    int kingDirections[8][2] =
    {
        {-1, -1},
        {-1, 0},
        {-1, +1},
        {0, -1},
        {0, +1},
        {+1, -1},
        {+1, 0},
        {+1, +1}
    };


    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            char piece = position->board[i][j];

            if (position->sideToMove == 0 && piece != 'K')
                continue;

            if (position->sideToMove == 1 && piece != 'k')
                continue;

            for (int direction = 0; direction < 8; direction++)
            {
                int newRow = i + kingDirections[direction][0];
                int newCol = j + kingDirections[direction][1];

                if (newRow < 0 || newRow >= 8 ||
                    newCol < 0 || newCol >= 8)
                {
                    continue;
                }

                char target = position->board[newRow][newCol];

                // Case occupée par une pièce alliée
                if (position->sideToMove == 0 && IsWhitePiece(target))
                    continue;

                if (position->sideToMove == 1 && IsBlackPiece(target))
                    continue;

                AddMove(moveList, i, j, newRow, newCol);
            }
        }
    }
}

int IsSquareAttacked(Position *position, int row, int col, int bySide)
{
    /*
     * --------------------------------------------------------
     * ATTAQUES DES PIONS
     * --------------------------------------------------------
     */

    if (bySide == 0) // Blancs
    {
        // Un pion blanc attaque row + 1 depuis la case cible
        if (row < 7)
        {
            if (col > 0 && position->board[row + 1][col - 1] == 'P')
                return 1;

            if (col < 7 && position->board[row + 1][col + 1] == 'P')
                return 1;
        }
    }
    else // Noirs
    {
        // Un pion noir attaque row - 1 depuis la case cible
        if (row > 0)
        {
            if (col > 0 && position->board[row - 1][col - 1] == 'p')
                return 1;

            if (col < 7 && position->board[row - 1][col + 1] == 'p')
                return 1;
        }
    }


    /*
     * --------------------------------------------------------
     * ATTAQUES DES CAVALIERS
     * --------------------------------------------------------
     */

    char knight = (bySide == 0) ? 'N' : 'n';

    int knightOffsets[8][2] =
    {
        {-2, -1},
        {-2,  1},
        {-1, -2},
        {-1,  2},
        { 1, -2},
        { 1,  2},
        { 2, -1},
        { 2,  1}
    };

    for (int i = 0; i < 8; i++)
    {
        int r = row + knightOffsets[i][0];
        int c = col + knightOffsets[i][1];

        if (r >= 0 && r < 8 && c >= 0 && c < 8)
        {
            if (position->board[r][c] == knight)
                return 1;
        }
    }


    /*
     * --------------------------------------------------------
     * ATTAQUES DU ROI
     * --------------------------------------------------------
     */

    char king = (bySide == 0) ? 'K' : 'k';

    for (int dr = -1; dr <= 1; dr++)
    {
        for (int dc = -1; dc <= 1; dc++)
        {
            if (dr == 0 && dc == 0)
                continue;

            int r = row + dr;
            int c = col + dc;

            if (r >= 0 && r < 8 && c >= 0 && c < 8)
            {
                if (position->board[r][c] == king)
                    return 1;
            }
        }
    }


    /*
     * --------------------------------------------------------
     * ATTAQUES DES FOUS / DAMES
     *
     * diagonales :
     * haut-gauche
     * haut-droite
     * bas-gauche
     * bas-droite
     * --------------------------------------------------------
     */

    char bishop = (bySide == 0) ? 'B' : 'b';
    char queen  = (bySide == 0) ? 'Q' : 'q';

    int diagonalDirections[4][2] =
    {
        {-1, -1},
        {-1,  1},
        { 1, -1},
        { 1,  1}
    };

    for (int i = 0; i < 4; i++)
    {
        int r = row + diagonalDirections[i][0];
        int c = col + diagonalDirections[i][1];

        while (r >= 0 && r < 8 && c >= 0 && c < 8)
        {
            char piece = position->board[r][c];

            if (piece != '.')
            {
                if (piece == bishop || piece == queen)
                    return 1;

                break;
            }

            r += diagonalDirections[i][0];
            c += diagonalDirections[i][1];
        }
    }


    /*
     * --------------------------------------------------------
     * ATTAQUES DES TOURS / DAMES
     *
     * horizontalement et verticalement
     * --------------------------------------------------------
     */

    char rook = (bySide == 0) ? 'R' : 'r';

    int straightDirections[4][2] =
    {
        {-1,  0},
        { 1,  0},
        { 0, -1},
        { 0,  1}
    };

    for (int i = 0; i < 4; i++)
    {
        int r = row + straightDirections[i][0];
        int c = col + straightDirections[i][1];

        while (r >= 0 && r < 8 && c >= 0 && c < 8)
        {
            char piece = position->board[r][c];

            if (piece != '.')
            {
                if (piece == rook || piece == queen)
                    return 1;

                break;
            }

            r += straightDirections[i][0];
            c += straightDirections[i][1];
        }
    }


    /*
     * --------------------------------------------------------
     * AUCUNE ATTAQUE
     * --------------------------------------------------------
     */

    return 0;
}

int IsInCheck(Position *position, int side)
{
    char king = (side == 0) ? 'K' : 'k';

    /*
     * Chercher le roi du joueur
     */
    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            if (position->board[row][col] == king)
            {
                /*
                 * Le roi est attaqué par l'adversaire
                 */
                int opponentSide = (side == 0) ? 1 : 0;

                return IsSquareAttacked(
                    position,
                    row,
                    col,
                    opponentSide
                );
            }
        }
    }

    /*
     * Aucun roi trouvé.
     *
     * Une position d'échecs valide doit toujours
     * contenir le roi.
     */
    return 0;
}

void CopyPosition(Position *source, Position *destination)
{
    *destination = *source;
}

void GenerateCastlingMoves(Position *position, MoveList *moveList)
{
    if (position->sideToMove == 0) // Blancs
    {
        // Petit roque (O-O) : e1 -> g1, tour h1 -> f1
        if (position->whiteKingSideCastle &&
            position->board[7][4] == 'K' &&
            position->board[7][7] == 'R' &&
            position->board[7][5] == '.' &&
            position->board[7][6] == '.' &&
            !IsSquareAttacked(position, 7, 4, 1) &&
            !IsSquareAttacked(position, 7, 5, 1) &&
            !IsSquareAttacked(position, 7, 6, 1))
        {
            AddMove(moveList, 7, 4, 7, 6);
        }

        // Grand roque (O-O-O) : e1 -> c1, tour a1 -> d1
        if (position->whiteQueenSideCastle &&
            position->board[7][4] == 'K' &&
            position->board[7][0] == 'R' &&
            position->board[7][1] == '.' &&
            position->board[7][2] == '.' &&
            position->board[7][3] == '.' &&
            !IsSquareAttacked(position, 7, 4, 1) &&
            !IsSquareAttacked(position, 7, 3, 1) &&
            !IsSquareAttacked(position, 7, 2, 1))
        {
            AddMove(moveList, 7, 4, 7, 2);
        }
    }
    else // Noirs
    {
        // Petit roque (O-O) : e8 -> g8, tour h8 -> f8
        if (position->blackKingSideCastle &&
            position->board[0][4] == 'k' &&
            position->board[0][7] == 'r' &&
            position->board[0][5] == '.' &&
            position->board[0][6] == '.' &&
            !IsSquareAttacked(position, 0, 4, 0) &&
            !IsSquareAttacked(position, 0, 5, 0) &&
            !IsSquareAttacked(position, 0, 6, 0))
        {
            AddMove(moveList, 0, 4, 0, 6);
        }

        // Grand roque (O-O-O) : e8 -> c8, tour a8 -> d8
        if (position->blackQueenSideCastle &&
            position->board[0][4] == 'k' &&
            position->board[0][0] == 'r' &&
            position->board[0][1] == '.' &&
            position->board[0][2] == '.' &&
            position->board[0][3] == '.' &&
            !IsSquareAttacked(position, 0, 4, 0) &&
            !IsSquareAttacked(position, 0, 3, 0) &&
            !IsSquareAttacked(position, 0, 2, 0))
        {
            AddMove(moveList, 0, 4, 0, 2);
        }
    }
}

void GeneratePseudoLegalMoves(Position *position, MoveList *moveList)
{
    moveList->count = 0;

    GeneratePawnMoves(position, moveList);
    GenerateKnightMoves(position, moveList);
    GenerateBishopMoves(position, moveList);
    GenerateRookMoves(position, moveList);
    GenerateQueenMoves(position, moveList);
    GenerateKingMoves(position, moveList);
    GenerateCastlingMoves(position, moveList);
}

void GenerateLegalMoves(Position *position, MoveList *legalMoves)
{
    MoveList pseudoMoves;
    Position copy;

    int mover = position->sideToMove;

    legalMoves->count = 0;

    GeneratePseudoLegalMoves(position, &pseudoMoves);

    for (int i = 0; i < pseudoMoves.count; i++)
    {
        CopyPosition(position, &copy);

        MakeMove(&copy, pseudoMoves.moves[i]);

        /*
         * Un coup est légal seulement si, après l'avoir joué,
         * notre propre roi n'est pas en échec.
         */
        if (!IsInCheck(&copy, mover))
        {
            legalMoves->moves[legalMoves->count] = pseudoMoves.moves[i];
            legalMoves->count++;
        }
    }
}

long long Perft(Position *position, int depth)
{
    if (depth == 0)
        return 1;

    MoveList legalMoves;
    GenerateLegalMoves(position, &legalMoves);

    if (depth == 1)
        return legalMoves.count;

    long long nodes = 0;
    Position copy;

    for (int i = 0; i < legalMoves.count; i++)
    {
        CopyPosition(position, &copy);
        MakeMove(&copy, legalMoves.moves[i]);
        nodes += Perft(&copy, depth - 1);
    }

    return nodes;
}

int IsCheckmate(Position *position)
{
    MoveList legalMoves;
    GenerateLegalMoves(position, &legalMoves);
 
    /*
     * Échec et mat = aucun coup légal disponible,
     * ET le joueur au trait est actuellement en échec.
     */
    return (legalMoves.count == 0) && IsInCheck(position, position->sideToMove);
}
 
int IsStalemate(Position *position)
{
    MoveList legalMoves;
    GenerateLegalMoves(position, &legalMoves);
 
    /*
     * Pat = aucun coup légal disponible,
     * MAIS le joueur au trait n'est PAS en échec.
     */
    return (legalMoves.count == 0) && !IsInCheck(position, position->sideToMove);
}