#include "search.h"
#include "eval.h"
#include <time.h>


#define INFINITY_SCORE 2000000000
#define MAX_KILLER_DEPTH 64

static volatile int searchStopped = 0;

static int g_timeLimited = 0;
static double g_deadline;
static int g_timeUp;
static long long g_nodeCount;
static int g_lastRootScore; // score du dernier FindBestMove (pour IterativeDeepening)

// "Coup precedent" : le meilleur coup trouve a la profondeur
// precedente d'IterativeDeepening, essaye en premier a la racine
// de la profondeur suivante (approximation simple d'un coup PV,
// sans table de transposition).
static Move g_pvMove;
static int g_hasPvMove = 0;

// Coups "killer" : coups tranquilles (non-captures) qui ont
// provoque une coupure beta a une profondeur donnee, ailleurs
// dans l'arbre. Essayes tot aux noeuds de meme profondeur.
static Move g_killerMoves[MAX_KILLER_DEPTH][2];
static int g_hasKiller[MAX_KILLER_DEPTH][2];


static double GetTimeSeconds(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (double)ts.tv_sec +
           (double)ts.tv_nsec / 1000000000.0;
}

static int TimeUp(void)
{
    if (searchStopped)
    {
        g_timeUp = 1;
        return 1;
    }

    if (!g_timeLimited)
        return 0;

    if (g_timeUp)
        return 1;

    g_nodeCount++;

    if ((g_nodeCount % 2048) == 0)
    {
        if (GetTimeSeconds() >= g_deadline)
        {
            g_timeUp = 1;
        }
    }

    return g_timeUp;
}

static void SetSearchDeadline(double seconds)
{
    g_timeLimited = 1;
    g_timeUp = 0;
    g_nodeCount = 0;

    g_deadline = GetTimeSeconds() + seconds;
}

static void ClearSearchDeadline(void)
{
    g_timeLimited = 0;
}

static int WasSearchInterrupted(void)
{
    return g_timeUp || searchStopped;
}

static int MovesEqual(Move a, Move b)
{
    return a.fromRow == b.fromRow && a.fromCol == b.fromCol &&
           a.toRow == b.toRow && a.toCol == b.toCol &&
           a.promotion == b.promotion && a.enPassant == b.enPassant;
}

static void ClearKillers(void)
{
    for (int d = 0; d < MAX_KILLER_DEPTH; d++)
    {
        g_hasKiller[d][0] = 0;
        g_hasKiller[d][1] = 0;
    }
}

static void StoreKiller(int depth, Move move)
{
    if (depth < 0 || depth >= MAX_KILLER_DEPTH)
    {
        return;
    }

    // Pas de doublon en tete
    if (g_hasKiller[depth][0] && MovesEqual(g_killerMoves[depth][0], move))
    {
        return;
    }

    g_killerMoves[depth][1] = g_killerMoves[depth][0];
    g_hasKiller[depth][1] = g_hasKiller[depth][0];

    g_killerMoves[depth][0] = move;
    g_hasKiller[depth][0] = 1;
}

static int IsKiller(int depth, Move move)
{
    if (depth < 0 || depth >= MAX_KILLER_DEPTH)
    {
        return 0;
    }

    return (g_hasKiller[depth][0] && MovesEqual(g_killerMoves[depth][0], move)) ||
           (g_hasKiller[depth][1] && MovesEqual(g_killerMoves[depth][1], move));
}

/*
 * Ordre de priorite du tri des coups :
 *   Coup precedent (PV)   extremement eleve
 *   Echec                 tres eleve
 *   Promotion              tres eleve
 *   Capture (MVV-LVA)      eleve
 *   Coup killer             eleve
 *   Autres coups             faible
 *
 * "Echec" est teste avec MakeMoveWithUndo/UndoMove : bien moins
 * cher qu'une copie complete de la Position.
 */
static int MoveOrderScore(Position *position, Move move, int depth)
{
    if (g_hasPvMove && MovesEqual(move, g_pvMove))
    {
        return 1000000; // extremement eleve
    }

    int score = 0;

    int isCapture = (position->board[move.toRow][move.toCol] != '.') || move.enPassant;

    if (isCapture)
    {
        char attacker = position->board[move.fromRow][move.fromCol];
        int victimValue = move.enPassant ? 100 : PieceValue(position->board[move.toRow][move.toCol]);
        score += 10000 + victimValue * 10 - PieceValue(attacker); // eleve
    }

    if (move.promotion != '\0')
    {
        score += 50000 + PieceValue(move.promotion); // tres eleve
    }

    UndoInfo undo = MakeMoveWithUndo(position, move);
    if (IsInCheck(position, position->sideToMove))
    {
        score += 50000; // tres eleve
    }
    UndoMove(position, move, undo);

    if (!isCapture && move.promotion == '\0' && IsKiller(depth, move))
    {
        score += 9000; // eleve
    }

    return score; // sinon : faible (0)
}

// Tri par insertion (liste courte, pas besoin de qsort) : place les
// coups les plus prometteurs en tete de liste, en place.
// depth = -1 pour desactiver le bonus "killer" (utilise en quiescence).
static void OrderMoves(Position *position, MoveList *moveList, int depth)
{
    static int scores[256];

    for (int i = 0; i < moveList->count; i++)
    {
        scores[i] = MoveOrderScore(position, moveList->moves[i], depth);
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

    OrderMoves(position, &captures, -1); // pas de killer en quiescence

    if (maximizingPlayer)
    {
        for (int i = 0; i < captures.count; i++)
        {
            UndoInfo undo = MakeMoveWithUndo(position, captures.moves[i]);
            int score = Quiescence(position, alpha, beta, 0);
            UndoMove(position, captures.moves[i], undo);

            if (score > alpha) alpha = score;
            if (alpha >= beta) break;
        }

        return alpha;
    }
    else
    {
        for (int i = 0; i < captures.count; i++)
        {
            UndoInfo undo = MakeMoveWithUndo(position, captures.moves[i]);
            int score = Quiescence(position, alpha, beta, 1);
            UndoMove(position, captures.moves[i], undo);

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

    if (maximizingPlayer)
    {
        int best = -INFINITY_SCORE;

        for (int i = 0; i < legalMoves.count; i++)
        {
            UndoInfo undo = MakeMoveWithUndo(position, legalMoves.moves[i]);
            int score = Minimax(position, depth - 1, 0);
            UndoMove(position, legalMoves.moves[i], undo);

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
            UndoInfo undo = MakeMoveWithUndo(position, legalMoves.moves[i]);
            int score = Minimax(position, depth - 1, 1);
            UndoMove(position, legalMoves.moves[i], undo);

            if (score < best)
            {
                best = score;
            }
        }

        return best;
    }
}

int AlphaBeta(
    Position *position,
    int depth,
    int alpha,
    int beta,
    int maximizingPlayer)
{

    if (WasSearchInterrupted())
    {
        return 0;
    }

    if (TimeUp())
    {
        return 0;
    }

    /*
     * --------------------------------------------------------
     * TABLE DE TRANSPOSITION
     * --------------------------------------------------------
     */

    int originalAlpha = alpha;

    int ttScore;
    Move ttMove;

    if (TT_Probe(
            position->hash,
            depth,
            alpha,
            beta,
            &ttScore,
            &ttMove))
    {
        return ttScore;
    }

    /*
     * --------------------------------------------------------
     * GENERATION DES COUPS
     * --------------------------------------------------------
     */

    MoveList legalMoves;

    GenerateLegalMoves(
        position,
        &legalMoves
    );

    /*
     * --------------------------------------------------------
     * POSITION TERMINALE
     * --------------------------------------------------------
     */

    if (legalMoves.count == 0)
    {
        return Evaluate(position);
    }

    /*
     * --------------------------------------------------------
     * QUIESCENCE
     * --------------------------------------------------------
     */

    if (depth == 0)
    {
        return Quiescence(
            position,
            alpha,
            beta,
            maximizingPlayer
        );
    }

    /*
     * --------------------------------------------------------
     * MOVE ORDERING
     * --------------------------------------------------------
     */

    OrderMoves(
        position,
        &legalMoves,
        depth
    );

    /*
     * --------------------------------------------------------
     * METTRE LE COUP DE LA TT EN PREMIER
     * --------------------------------------------------------
     */

    if (ttMove.fromRow >= 0)
    {
        for (int i = 0;
             i < legalMoves.count;
             i++)
        {
            if (MovesEqual(
                    legalMoves.moves[i],
                    ttMove))
            {
                Move temp =
                    legalMoves.moves[0];

                legalMoves.moves[0] =
                    legalMoves.moves[i];

                legalMoves.moves[i] =
                    temp;

                break;
            }
        }
    }

    /*
     * --------------------------------------------------------
     * MAX
     * --------------------------------------------------------
     */

    if (maximizingPlayer)
    {
        int best = -INFINITY_SCORE;

        Move bestMove = legalMoves.moves[0];

        for (int i = 0;
             i < legalMoves.count;
             i++)
        {
            Move move =
                legalMoves.moves[i];

            UndoInfo undo =
                MakeMoveWithUndo(
                    position,
                    move
                );

            int score =
                AlphaBeta(
                    position,
                    depth - 1,
                    alpha,
                    beta,
                    0
                );

            UndoMove(
                position,
                move,
                undo
            );

            if (score > best)
            {
                best = score;
                bestMove = move;
            }

            if (best > alpha)
            {
                alpha = best;
            }

            if (alpha >= beta)
            {
                int isCapture =
                    (position->board[
                        move.toRow
                    ][
                        move.toCol
                    ] != '.') ||
                    move.enPassant;

                if (!isCapture &&
                    move.promotion == '\0')
                {
                    StoreKiller(
                        depth,
                        move
                    );
                }

                break;
            }
        }

        /*
         * ----------------------------------------------------
         * STOCKAGE TT
         * (jamais si le temps est ecoule : "best" serait
         * contamine par le 0 factice remonte par TimeUp())
         * ----------------------------------------------------
         */

        if (!WasSearchInterrupted())
        {
            TTFlag flag;

            if (best <= originalAlpha)
            {
                flag = TT_ALPHA;
            }
            else if (best >= beta)
            {
                flag = TT_BETA;
            }
            else
            {
                flag = TT_EXACT;
            }

            TT_Store(
                position->hash,
                depth,
                best,
                flag,
                bestMove
            );
        }

        return best;
    }

    /*
     * --------------------------------------------------------
     * MIN
     * --------------------------------------------------------
     */

    else
    {
        int best = INFINITY_SCORE;

        Move bestMove = legalMoves.moves[0];

        for (int i = 0;
             i < legalMoves.count;
             i++)
        {
            Move move =
                legalMoves.moves[i];

            UndoInfo undo =
                MakeMoveWithUndo(
                    position,
                    move
                );

            int score =
                AlphaBeta(
                    position,
                    depth - 1,
                    alpha,
                    beta,
                    1
                );

            UndoMove(
                position,
                move,
                undo
            );

            if (score < best)
            {
                best = score;
                bestMove = move;
            }

            if (best < beta)
            {
                beta = best;
            }

            if (alpha >= beta)
            {
                int isCapture =
                    (position->board[
                        move.toRow
                    ][
                        move.toCol
                    ] != '.') ||
                    move.enPassant;

                if (!isCapture &&
                    move.promotion == '\0')
                {
                    StoreKiller(
                        depth,
                        move
                    );
                }

                break;
            }
        }

        /*
         * ----------------------------------------------------
         * STOCKAGE TT
         * (jamais si le temps est ecoule)
         * ----------------------------------------------------
         */

        if (!WasSearchInterrupted())
        {
            TTFlag flag;

            if (best <= originalAlpha)
            {
                flag = TT_ALPHA;
            }
            else if (best >= beta)
            {
                flag = TT_BETA;
            }
            else
            {
                flag = TT_EXACT;
            }

            TT_Store(
                position->hash,
                depth,
                best,
                flag,
                bestMove
            );
        }

        return best;
    }
}

Move FindBestMove(Position *position, int depth)
{
    MoveList legalMoves;
    GenerateLegalMoves(position, &legalMoves);

    OrderMoves(position, &legalMoves, depth);

    int maximizingPlayer = (position->sideToMove == 0);

    Move bestMove = legalMoves.moves[0]; // defaut
    int bestScore = maximizingPlayer ? -INFINITY_SCORE : INFINITY_SCORE;

    for (int i = 0; i < legalMoves.count; i++)
    {
        Move move = legalMoves.moves[i];

        UndoInfo undo = MakeMoveWithUndo(position, move);
        int score = AlphaBeta(position, depth - 1, -INFINITY_SCORE, INFINITY_SCORE, !maximizingPlayer);
        UndoMove(position, move, undo);

        if (maximizingPlayer && score > bestScore)
        {
            bestScore = score;
            bestMove = move;
        }
        else if (!maximizingPlayer && score < bestScore)
        {
            bestScore = score;
            bestMove = move;
        }
    }

    g_lastRootScore = bestScore; // pour IterativeDeepening

    return bestMove;
}

SearchResult IterativeDeepening(
    Position *position,
    int maxDepth,
    double timeLimitSeconds,
    SearchProgressCallback onProgress)
{
    /*
     * Nouvelle recherche :
     * on autorise à nouveau la recherche.
     */
    searchStopped = 0;

    /*
     * Initialisation du temps de recherche.
     */
    SetSearchDeadline(timeLimitSeconds);

    /*
     * On ne vide PAS la TT ici.
     *
     * La TT doit être conservée entre les
     * différentes profondeurs de l'iterative deepening.
     */
    ClearKillers();

    g_hasPvMove = 0;

    /*
     * Génération des coups légaux.
     */
    MoveList legalMoves;

    GenerateLegalMoves(
        position,
        &legalMoves
    );

    /*
     * Sécurité :
     * aucune position légale.
     */
    SearchResult result;

    if (legalMoves.count == 0)
    {
        result.move = (Move){0};
        result.score = Evaluate(position);
        result.depth = 0;

        ClearSearchDeadline();

        return result;
    }

    /*
     * Résultat de secours.
     *
     * Tant qu'aucune profondeur complète n'a été
     * terminée, on conserve le premier coup légal.
     */
    result.move = legalMoves.moves[0];
    result.score = Evaluate(position);
    result.depth = 0;


    /*
     * ========================================================
     * ITERATIVE DEEPENING
     * ========================================================
     */

    for (int depth = 1; depth <= maxDepth; depth++)
    {
        /*
         * Si le temps est écoulé ou si UCI a demandé
         * l'arrêt, on arrête avant de commencer
         * une nouvelle profondeur.
         */
        if (WasSearchInterrupted())
        {
            break;
        }

        /*
         * Recherche complète à cette profondeur.
         */
        Move candidate =
            FindBestMove(
                position,
                depth
            );

        /*
         * IMPORTANT :
         *
         * Si la recherche a été interrompue pendant
         * cette profondeur, on NE valide PAS candidate.
         *
         * On garde le résultat de la profondeur précédente.
         */
        if (WasSearchInterrupted())
        {
            break;
        }

        /*
         * La profondeur est entièrement terminée.
         * On valide donc le nouveau résultat.
         */
        result.move = candidate;
        result.score = g_lastRootScore;
        result.depth = depth;

        /*
         * Le meilleur coup trouvé devient le PV move
         * pour la prochaine profondeur.
         */
        g_pvMove = candidate;
        g_hasPvMove = 1;

        if (onProgress != NULL)
        {
            onProgress(depth, g_lastRootScore, candidate);
        }
    }


    /*
     * Nettoyage de la deadline.
     */
    ClearSearchDeadline();

    return result;
}


void SearchStop(void)
{
    searchStopped = 1;
}


void SearchResetStop(void)
{
    searchStopped = 0;
}