#include "search.h"
#include "eval.h"
#include <time.h>


#define INFINITY_SCORE 2000000000

static int g_timeLimited = 0;
static clock_t g_deadline;
static int g_timeUp;
static long long g_nodeCount;

static int TimeUp(void)
{
    if (!g_timeLimited) return 0;
    if (g_timeUp) return 1;

    g_nodeCount++;

    if ((g_nodeCount % 2048) == 0 && clock() >= g_deadline)
    {
        g_timeUp = 1;
    }

    return g_timeUp;
}

static void SetSearchDeadline(double seconds)
{
    g_timeLimited = 1;
    g_timeUp = 0;
    g_nodeCount = 0;
    g_deadline = clock() + (clock_t)(seconds * CLOCKS_PER_SEC);
}

static void ClearSearchDeadline(void)
{
    g_timeLimited = 0;
}

static int WasSearchInterrupted(void)
{
    return g_timeUp;
}

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

    if (TimeUp())
    {
        return 0;
    }

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

Move IterativeDeepening(Position *position, int maxDepth, double timeLimitSeconds)
{
    SetSearchDeadline(timeLimitSeconds);

    MoveList legalMoves;
    GenerateLegalMoves(position, &legalMoves);
    Move bestMove = legalMoves.moves[0];

    for (int depth = 1; depth <= maxDepth; depth++)
    {
        Move candidate = FindBestMove(position, depth);

        if (WasSearchInterrupted())
        {
            break;
        }

        bestMove = candidate;
    }

    ClearSearchDeadline();

    return bestMove;
}