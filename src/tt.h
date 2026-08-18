#ifndef TT_H
#define TT_H

#include <stdint.h>
#include "move.h"

typedef enum
{
    TT_EXACT,
    TT_ALPHA,
    TT_BETA
} TTFlag;

typedef struct
{
    uint64_t key;

    int depth;
    int score;

    TTFlag flag;

    Move bestMove;

    int used;

} TTEntry;

void TT_Init(int sizeMB);
void TT_Clear(void);

int TT_Probe(
    uint64_t key,
    int depth,
    int alpha,
    int beta,
    int *score,
    Move *bestMove
);

void TT_Store(
    uint64_t key,
    int depth,
    int score,
    TTFlag flag,
    Move bestMove
);
void TT_PrintStats(void);
void TT_ResetStats(void);

#endif