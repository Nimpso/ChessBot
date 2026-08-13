#include "eval.h"
#include "move.h"


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

    if (IsCheckmate(position))
    {

        return (position->sideToMove == 0) ? -MATE_SCORE : MATE_SCORE;
    }

    if (IsStalemate(position))
    {
        return 0; // Nulle
    }

    // pour l instant tres simple on additionne juste les pieces
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