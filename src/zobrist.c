#include "zobrist.h"
#include <stdint.h>

static uint64_t pieceKeys[12][64];
static uint64_t sideKey;
static uint64_t castleKeys[4];
static uint64_t enPassantKeys[8];

static uint64_t randomState = 0x9E3779B97F4A7C15ULL;

static uint64_t Random64(void)
{
    randomState ^= randomState >> 12;
    randomState ^= randomState << 25;
    randomState ^= randomState >> 27;

    return randomState * 0x2545F4914F6CDD1DULL;
}

static int PieceIndex(char piece)
{
    switch (piece)
    {
        case 'P': return 0;
        case 'N': return 1;
        case 'B': return 2;
        case 'R': return 3;
        case 'Q': return 4;
        case 'K': return 5;

        case 'p': return 6;
        case 'n': return 7;
        case 'b': return 8;
        case 'r': return 9;
        case 'q': return 10;
        case 'k': return 11;
    }

    return -1;
}

void InitZobrist(void)
{
    randomState = 0x9E3779B97F4A7C15ULL;

    for (int piece = 0; piece < 12; piece++)
    {
        for (int square = 0; square < 64; square++)
        {
            pieceKeys[piece][square] = Random64();
        }
    }

    sideKey = Random64();

    for (int i = 0; i < 4; i++)
    {
        castleKeys[i] = Random64();
    }

    for (int i = 0; i < 8; i++)
    {
        enPassantKeys[i] = Random64();
    }
}

uint64_t GetPieceHash(char piece, int row, int col)
{
    int index = PieceIndex(piece);

    if (index < 0)
        return 0;

    return pieceKeys[index][row * 8 + col];
}

uint64_t GetSideHash(void)
{
    return sideKey;
}

uint64_t GetCastleHash(int castleIndex)
{
    if (castleIndex < 0 || castleIndex >= 4)
        return 0;

    return castleKeys[castleIndex];
}

uint64_t GetEnPassantHash(int col)
{
    if (col < 0 || col >= 8)
        return 0;

    return enPassantKeys[col];
}

uint64_t ComputeZobristHash(Position *position)
{
    uint64_t hash = 0;

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            char piece = position->board[row][col];

            if (piece != '.')
            {
                hash ^= GetPieceHash(piece, row, col);
            }
        }
    }

    if (position->sideToMove == 1)
    {
        hash ^= GetSideHash();
    }

    if (position->whiteKingSideCastle)
        hash ^= GetCastleHash(0);

    if (position->whiteQueenSideCastle)
        hash ^= GetCastleHash(1);

    if (position->blackKingSideCastle)
        hash ^= GetCastleHash(2);

    if (position->blackQueenSideCastle)
        hash ^= GetCastleHash(3);

    if (position->enPassantCol >= 0)
    {
        hash ^= GetEnPassantHash(position->enPassantCol);
    }

    return hash;
}