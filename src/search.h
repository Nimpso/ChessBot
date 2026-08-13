#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include "move.h"

/*
 * Explore l'arbre des coups jusqu'à "depth" demi-coups et
 * retourne le meilleur score atteignable (du point de vue
 * des BLANCS, comme Evaluate()).
 *
 * maximizingPlayer = 1 si c'est aux blancs de choisir
 * (ils veulent le score le PLUS GRAND),
 * 0 si c'est aux noirs de choisir
 * (ils veulent le score le PLUS PETIT).
 */
int Minimax(Position *position, int depth, int maximizingPlayer);

/*
 * Cherche le meilleur coup à jouer pour le joueur au trait
 * de "position", en explorant "depth" demi-coups avec Minimax.
 */
Move FindBestMove(Position *position, int depth);

#endif