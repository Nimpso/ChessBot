#include "search.h"
#include "eval.h"


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

int AlphaBeta(Position *position, int depth, int alpha, int beta, int maximizingPlayer)
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

            int score = AlphaBeta(&copy, depth - 1, alpha, beta, 0);

            if (score > best)
            {
                best = score;
            }

            if (best > alpha)
            {
                alpha = best;
            }

            if (alpha >= beta)
            {
                break; // coupure beta
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

            int score = AlphaBeta(&copy, depth - 1, alpha, beta, 1);

            if (score < best)
            {
                best = score;
            }

            if (best < beta)
            {
                beta = best;
            }

            if (alpha >= beta)
            {
                break; // coupure alpha
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

        int score = AlphaBeta(&copy, depth - 1, -INFINITY_SCORE, INFINITY_SCORE, !maximizingPlayer);

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