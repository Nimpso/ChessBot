#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include "move.h"
#include "tt.h"

/*
 * Explore l'arbre des coups jusqu'à "depth" demi-coups et
 * retourne le meilleur score atteignable pdv blancs
 */
int Minimax(Position *position, int depth, int maximizingPlayer);
// Comme Minimax, mais coupe les branches inutiles (meme resultat, plus rapide)
int AlphaBeta(Position *position, int depth, int alpha, int beta, int maximizingPlayer);

/*
 * Cherche le meilleur coup à jouer pour le joueur au trait
 * de "position", en explorant "depth" demi-coups avec Minimax.
 */
Move FindBestMove(Position *position, int depth);
// Poursuit l'exploration des captures au-dela de la profondeur 0,
// pour ne pas evaluer une position en plein milieu d'un echange.
int Quiescence(Position *position, int alpha, int beta, int maximizingPlayer);


typedef struct
{
    Move move;
    int score;
    int depth;
} SearchResult;

// Appele apres chaque profondeur d'IterativeDeepening entierement
// terminee (score toujours pdv blancs, comme le reste du moteur).
// NULL si l'appelant n'a pas besoin de suivre la progression.
typedef void (*SearchProgressCallback)(int depth, int score, Move move);

SearchResult IterativeDeepening(
    Position *position,
    int maxDepth,
    double timeLimitSeconds,
    SearchProgressCallback onProgress
);

void SearchStop(void);
void SearchResetStop(void);
#endif