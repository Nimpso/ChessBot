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

// Roi en finale : au contraire, un roi actif au centre est un atout
// (il aide a pousser les pions et a soutenir l'attaque).
static const int kingEndgameTable[8][8] = {
    {-50,-40,-30,-20,-20,-30,-40,-50},
    {-30,-20,-10,  0,  0,-10,-20,-30},
    {-30,-10, 20, 30, 30, 20,-10,-30},
    {-30,-10, 30, 40, 40, 30,-10,-30},
    {-30,-10, 30, 40, 40, 30,-10,-30},
    {-30,-10, 20, 30, 30, 20,-10,-30},
    {-30,-30,  0,  0,  0,  0,-30,-30},
    {-50,-30,-30,-30,-30,-30,-30,-50}
};

// Pions en finale : l'avancee vers la promotion compte beaucoup plus
// qu'en milieu de partie, ou la structure/le centre priment.
static const int pawnEndgameTable[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0},
    { 80, 80, 80, 80, 80, 80, 80, 80},
    { 50, 50, 50, 50, 50, 50, 50, 50},
    { 30, 30, 30, 30, 30, 30, 30, 30},
    { 20, 20, 20, 20, 20, 20, 20, 20},
    { 10, 10, 10, 10, 10, 10, 10, 10},
    { 10, 10, 10, 10, 10, 10, 10, 10},
    {  0,  0,  0,  0,  0,  0,  0,  0}
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

// Poids de "phase" par piece (independant de PieceValue, qui sert au
// materiel). Total au depart : 2*(2+2+4+4) = 24 -> ouverture.
// Plus aucune piece majeure/mineure sur le plateau -> 0 -> finale.
static int PiecePhaseWeight(char piece)
{
    switch (piece)
    {
        case 'N': case 'n': return 1;
        case 'B': case 'b': return 1;
        case 'R': case 'r': return 2;
        case 'Q': case 'q': return 4;
        default: return 0;
    }
}

int GamePhase(Position *position)
{
    int phase = 0;

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            phase += PiecePhaseWeight(position->board[row][col]);
        }
    }

    if (phase > 24)
        phase = 24; // securite si jamais une promotion multiplie les dames

    return phase;
}

// Melange mgValue et egValue selon la phase (24 = tout milieu de
// partie, 0 = tout finale).
static int Interpolate(int mgValue, int egValue, int phase)
{
    return (mgValue * phase + egValue * (24 - phase)) / 24;
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
     * pour les noirs. Le roi et les pions utilisent une table
     * qui depend de la phase de la partie (ouverture/milieu de
     * partie -> finale).
     */
    int phase = GamePhase(position);
    int score = 0;

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            char piece = position->board[row][col];

            if (piece == '.')
                continue;

            int r = IsWhitePiece(piece) ? row : 7 - row;
            int positionalValue;

            if (piece == 'K' || piece == 'k')
            {
                positionalValue = Interpolate(kingTable[r][col], kingEndgameTable[r][col], phase);
            }
            else if (piece == 'P' || piece == 'p')
            {
                positionalValue = Interpolate(pawnTable[r][col], pawnEndgameTable[r][col], phase);
            }
            else
            {
                positionalValue = PieceSquareValue(piece, row, col);
            }

            int value = PieceValue(piece) + positionalValue;

            if (IsWhitePiece(piece))
                score += value;
            else
                score -= value;
        }
    }

    return score;
}