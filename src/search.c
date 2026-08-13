#include "search.h"
#include "eval.h"

/*
 * Sentinelle "infini" pour initialiser la recherche du meilleur
 * score. Volontairement bien plus grande que MATE_SCORE (défini
 * dans eval.c), pour qu'aucun score réel ne puisse jamais
 * l'atteindre ou le dépasser.
 */
#define INFINITY_SCORE 2000000000

int Minimax(Position *position, int depth, int maximizingPlayer)
{
    MoveList legalMoves;
    GenerateLegalMoves(position, &legalMoves);

    if (legalMoves.count == 0)
    {
        return Evaluate(position);
    }

    if (depth == 0)
    {
        return Evaluate(position);
    }

    Position copy;

    if (maximizingPlayer)
    {
        int best = -INFINITY_SCORE;

        for (int i = 0; i < legalMoves.count; i++)
        {
            CopyPosition(position, &copy);
            MakeMove(&copy, legalMoves.moves[i]);

            int score = Minimax(&copy, depth - 1, 0);

            if (score > best)
            {
                best = score;
            }
        }

        return best;
    }
    else
    {
        int best = INFINITY_SCORE;

        for (int i = 0; i < legalMoves.count; i++)
        {
            CopyPosition(position, &copy);
            MakeMove(&copy, legalMoves.moves[i]);

            int score = Minimax(&copy, depth - 1, 1);

            if (score < best)
            {
                best = score;
            }
        }

        return best;
    }
}

Move FindBestMove(Position *position, int depth)
{
    MoveList legalMoves;
    GenerateLegalMoves(position, &legalMoves);


    int maximizingPlayer = (position->sideToMove == 0);

    Move bestMove = legalMoves.moves[0]; // defaut
    int bestScore = maximizingPlayer ? -INFINITY_SCORE : INFINITY_SCORE;

    Position copy;

    for (int i = 0; i < legalMoves.count; i++)
    {
        CopyPosition(position, &copy);
        MakeMove(&copy, legalMoves.moves[i]);

        int score = Minimax(&copy, depth - 1, !maximizingPlayer);

        if (maximizingPlayer && score > bestScore)
        {
            bestScore = score;
            bestMove = legalMoves.moves[i];
        }
        else if (!maximizingPlayer && score < bestScore)
        {
            bestScore = score;
            bestMove = legalMoves.moves[i];
        }
    }

    return bestMove;
}