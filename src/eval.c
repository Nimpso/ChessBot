#include "eval.h"
#include "move.h"

/*
 * Score utilisé pour un mat. Volontairement bien plus grand
 * que n'importe quelle somme de matériel possible (le
 * matériel total sur l'échiquier ne dépasse jamais ~9000
 * centipions), pour que Minimax préfère TOUJOURS un mat à
 * n'importe quel gain de matériel.
 */
#define MATE_SCORE 1000000

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

            int value = PieceValue(piece);

            if (IsWhitePiece(piece))
                score += value;
            else
                score -= value;
        }
    }

    return score;
}