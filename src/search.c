#include "search.h"
#include "eval.h"
#include <time.h>


#define INFINITY_SCORE 2000000000

static int g_timeLimited = 0;
static clock_t g_deadline;
static int g_timeUp;
static long long g_nodeCount;
static int g_lastRootScore; // score du dernier FindBestMove (pour IterativeDeepening)

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

// MVV-LVA (Most Valuable Victim - Least Valuable Attacker) : les
// captures de grosses pieces par de petites pieces sont examinees
// en premier. Les promotions aussi. Un bon tri fait couper beaucoup
// plus de branches a AlphaBeta.
static int MoveOrderScore(Position *position, Move move)
{
    int score = 0;

    char capturedPiece = position->board[move.toRow][move.toCol];

    if (capturedPiece != '.')
    {
        char attacker = position->board[move.fromRow][move.fromCol];
        score += 10000 + PieceValue(capturedPiece) * 10 - PieceValue(attacker);
    }
    else if (move.enPassant)
    {
        score += 10000 + 100 * 10 - 100; // pion prend pion
    }

    if (move.promotion != '\0')
    {
        score += 9000 + PieceValue(move.promotion);
    }

    return score;
}

// Tri par insertion (liste courte, pas besoin de qsort) : place les
// coups les plus prometteurs en tete de liste, en place.
static void OrderMoves(Position *position, MoveList *moveList)
{
    static int scores[256];

    for (int i = 0; i < moveList->count; i++)
    {
        scores[i] = MoveOrderScore(position, moveList->moves[i]);
    }

    for (int i = 1; i < moveList->count; i++)
    {
        Move keyMove = moveList->moves[i];
        int keyScore = scores[i];
        int j = i - 1;

        while (j >= 0 && scores[j] < keyScore)
        {
            moveList->moves[j + 1] = moveList->moves[j];
            scores[j + 1] = scores[j];
            j--;
        }

        moveList->moves[j + 1] = keyMove;
        scores[j + 1] = keyScore;
    }
}

// Recherche de quiescence : au lieu de s'arreter net a profondeur 0,
// continue a explorer les captures (et promotions) jusqu'a une
// position "calme". Evite l'effet d'horizon : s'arreter pile au
// milieu d'un echange de pieces et se tromper sur qui gagne le
// materiel.
int Quiescence(Position *position, int alpha, int beta, int maximizingPlayer)
{
    if (TimeUp())
    {
        return 0;
    }

    int standPat = Evaluate(position);

    // "stand pat" : le score si on ne capture plus rien. Sert de
    // plancher/plafond, comme dans AlphaBeta normal.
    if (maximizingPlayer)
    {
        if (standPat >= beta) return beta;
        if (standPat > alpha) alpha = standPat;
    }
    else
    {
        if (standPat <= alpha) return alpha;
        if (standPat < beta) beta = standPat;
    }

    MoveList legalMoves;
    GenerateLegalMoves(position, &legalMoves);

    if (legalMoves.count == 0)
    {
        return standPat; // mat/pat deja pris en compte dans Evaluate()
    }

    // Ne garder que les captures et promotions
    MoveList captures;
    captures.count = 0;

    for (int i = 0; i < legalMoves.count; i++)
    {
        Move m = legalMoves.moves[i];
        int isCapture = (position->board[m.toRow][m.toCol] != '.') || m.enPassant;

        if (isCapture || m.promotion != '\0')
        {
            captures.moves[captures.count] = m;
            captures.count++;
        }
    }

    OrderMoves(position, &captures);

    Position copy;

    if (maximizingPlayer)
    {
        for (int i = 0; i < captures.count; i++)
        {
            CopyPosition(position, &copy);
            MakeMove(&copy, captures.moves[i]);

            int score = Quiescence(&copy, alpha, beta, 0);

            if (score > alpha) alpha = score;
            if (alpha >= beta) break;
        }

        return alpha;
    }
    else
    {
        for (int i = 0; i < captures.count; i++)
        {
            CopyPosition(position, &copy);
            MakeMove(&copy, captures.moves[i]);

            int score = Quiescence(&copy, alpha, beta, 1);

            if (score < beta) beta = score;
            if (alpha >= beta) break;
        }

        return beta;
    }
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
        return Quiescence(position, alpha, beta, maximizingPlayer);
    }

    OrderMoves(position, &legalMoves);

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

    OrderMoves(position, &legalMoves);

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

    g_lastRootScore = bestScore; // pour IterativeDeepening

    return bestMove;
}

SearchResult IterativeDeepening(Position *position, int maxDepth, double timeLimitSeconds)
{
    SetSearchDeadline(timeLimitSeconds);

    MoveList legalMoves;
    GenerateLegalMoves(position, &legalMoves);

    SearchResult result;
    result.move = legalMoves.moves[0];
    result.score = Evaluate(position);
    result.depth = 0;

    for (int depth = 1; depth <= maxDepth; depth++)
    {
        Move candidate = FindBestMove(position, depth);

        if (WasSearchInterrupted())
        {
            break;
        }

        result.move = candidate;
        result.score = g_lastRootScore;
        result.depth = depth;
    }

    ClearSearchDeadline();

    return result;
}