#include "eval.h"
#include "move.h"

#define MATE_SCORE 1000000

// Tables positionnelles (bonus/malus par case), point de vue blanc.
// row 0 = rangee 8, row 7 = rangee 1.

static const int pawnTable[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0},
    { 50, 50, 50, 50, 50, 50, 50, 50},
    { 10, 10, 20, 30, 30, 20, 10, 10},
    {  5,  5, 10, 25, 25, 10,  5,  5},
    {  0,  0,  0, 20, 20,  0,  0,  0},
    {  5, -5,-10,  0,  0,-10, -5,  5},
    {  5, 10, 10,-20,-20, 10, 10,  5},
    {  0,  0,  0,  0,  0,  0,  0,  0}
};

static const int knightTable[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  0,  0,  0,-20,-40},
    {-30,  0, 10, 15, 15, 10,  0,-30},
    {-30,  5, 15, 20, 20, 15,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-30,-30,-30,-30,-40,-50}
};

static const int bishopTable[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5, 10, 10,  5,  0,-10},
    {-10,  5,  5, 10, 10,  5,  5,-10},
    {-10,  0, 10, 10, 10, 10,  0,-10},
    {-10, 10, 10, 10, 10, 10, 10,-10},
    {-10,  5,  0,  0,  0,  0,  5,-10},
    {-20,-10,-10,-10,-10,-10,-10,-20}
};

static const int rookTable[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0},
    {  5, 10, 10, 10, 10, 10, 10,  5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    {  0,  0,  0,  5,  5,  0,  0,  0}
};

static const int queenTable[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5,  5,  5,  5,  0,-10},
    { -5,  0,  5,  5,  5,  5,  0, -5},
    {  0,  0,  5,  5,  5,  5,  0, -5},
    {-10,  5,  5,  5,  5,  5,  0,-10},
    {-10,  0,  5,  0,  0,  0,  0,-10},
    {-20,-10,-10, -5, -5,-10,-10,-20}
};

// Roi en milieu de partie : reste en securite derriere ses pions.
static const int kingTable[8][8] = {
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-20,-30,-30,-40,-40,-30,-30,-20},
    {-10,-20,-20,-20,-20,-20,-20,-10},
    { 20, 20,  0,  0,  0,  0, 20, 20},
    { 20, 30, 10,  0,  0, 10, 30, 20}
};

int PieceSquareValue(char piece, int row, int col)
{
    // Les tables sont ecrites du point de vue blanc, on les
    // retourne verticalement pour les noirs (symetrie du jeu).
    int r = IsWhitePiece(piece) ? row : 7 - row;

    switch (piece)
    {
        case 'P': case 'p': return pawnTable[r][col];
        case 'N': case 'n': return knightTable[r][col];
        case 'B': case 'b': return bishopTable[r][col];
        case 'R': case 'r': return rookTable[r][col];
        case 'Q': case 'q': return queenTable[r][col];
        case 'K': case 'k': return kingTable[r][col];
        default: return 0;
    }
}

int PieceValue(char piece)
{
    switch (piece)
    {
        case 'P': case 'p': return 100;
        case 'N': case 'n': return 320;
        case 'B': case 'b': return 330;
        case 'R': case 'r': return 500;
        case 'Q': case 'q': return 900;
        default: return 0; // '.', 'K', 'k' : le roi n'a pas de valeur matérielle
    }
}

int Evaluate(Position *position)
{
    /*
     * D'abord les cas terminaux : un mat ou un pat rendent
     * le score matériel non pertinent.
     */
    if (IsCheckmate(position))
    {
        /*
         * Le joueur AU TRAIT est maté (il ne peut plus jouer).
         * Si c'est aux blancs de jouer, c'est donc une
         * catastrophe pour les blancs : score très négatif.
         */
        return (position->sideToMove == 0) ? -MATE_SCORE : MATE_SCORE;
    }

    if (IsStalemate(position))
    {
        return 0; // Nulle
    }

    if (IsFiftyMoveRule(position))
    {
        return 0; // Nulle (50 coups sans capture ni coup de pion)
    }

    /*
     * Évaluation matérielle simple : on additionne la valeur
     * de chaque pièce, en positif pour les blancs, en négatif
     * pour les noirs.
     */
    int score = 0;

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            char piece = position->board[row][col];

            if (piece == '.')
                continue;

            int value = PieceValue(piece) + PieceSquareValue(piece, row, col);

            if (IsWhitePiece(piece))
                score += value;
            else
                score -= value;
        }
    }

    return score;
}