#include "tt.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static TTEntry *table = NULL;
static size_t tableSize = 0;

static long long ttProbes = 0;
static long long ttHits = 0;
static long long ttStores = 0;

void TT_Init(int sizeMB)
{
    if (table != NULL)
    {
        free(table);
        table = NULL;
    }

    size_t bytes =
        (size_t)sizeMB * 1024 * 1024;

    tableSize =
        bytes / sizeof(TTEntry);

    if (tableSize == 0)
        tableSize = 1;

    table =
        calloc(tableSize, sizeof(TTEntry));
}

void TT_Clear(void)
{
    if (table != NULL)
    {
        memset(
            table,
            0,
            tableSize * sizeof(TTEntry)
        );
    }
}

static TTEntry *GetEntry(uint64_t key)
{
    if (table == NULL || tableSize == 0)
        return NULL;

    size_t index =
        key % tableSize;

    return &table[index];
}

int TT_Probe(
    uint64_t key,
    int depth,
    int alpha,
    int beta,
    int *score,
    Move *bestMove)
{
    ttProbes++;

    if (bestMove != NULL)
    {
        bestMove->fromRow = -1;
        bestMove->fromCol = -1;
        bestMove->toRow = -1;
        bestMove->toCol = -1;
        bestMove->promotion = '\0';
        bestMove->enPassant = 0;
    }

    TTEntry *entry = GetEntry(key);

    if (entry == NULL)
        return 0;

    if (!entry->used)
        return 0;

    if (entry->key != key)
        return 0;

    if (bestMove != NULL)
        *bestMove = entry->bestMove;

    if (entry->depth < depth)
        return 0;

    if (entry->flag == TT_EXACT)
    {
        ttHits++;
        *score = entry->score;
        return 1;
    }

    if (entry->flag == TT_ALPHA &&
        entry->score <= alpha)
    {
        ttHits++;
        *score = entry->score;
        return 1;
    }

    if (entry->flag == TT_BETA &&
        entry->score >= beta)
    {
        ttHits++;
        *score = entry->score;
        return 1;
    }

    return 0;
}

void TT_Store(
    uint64_t key,
    int depth,
    int score,
    TTFlag flag,
    Move bestMove)
{
  
    TTEntry *entry = GetEntry(key);
    ttStores++;

    if (entry == NULL)
        return;

    /*
     * Pour commencer :
     * on remplace l'entrée existante si elle est
     * moins profonde ou inutilisée.
     */

    if (!entry->used ||
        depth >= entry->depth ||
        entry->key == key)
    {
        entry->key = key;
        entry->depth = depth;
        entry->score = score;
        entry->flag = flag;
        entry->bestMove = bestMove;
        entry->used = 1;
    }
}

void TT_PrintStats(void)
{
    printf("\n--- TRANSPOSITION TABLE ---\n");
    printf("Probes : %lld\n", ttProbes);
    printf("Hits   : %lld\n", ttHits);
    printf("Stores : %lld\n", ttStores);

    if (ttProbes > 0)
    {
        printf(
            "Hit rate : %.2f%%\n",
            100.0 * (double)ttHits /
            (double)ttProbes
        );
    }
}

void TT_ResetStats(void)
{
    ttProbes = 0;
    ttHits = 0;
    ttStores = 0;
}