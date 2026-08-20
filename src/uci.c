#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "uci.h"
#include "board.h"
#include "move.h"
#include "search.h"
#include "zobrist.h"
#include "tt.h"


static Position position;


#define QUEUE_CAPACITY 256
#define LINE_MAX_LEN 4096

typedef struct
{
    char lines[QUEUE_CAPACITY][LINE_MAX_LEN];
    int head, tail, count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} CommandQueue;

static CommandQueue g_queue;

static void Queue_Init(CommandQueue *q)
{
    q->head = q->tail = q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
}

static void Queue_Push(CommandQueue *q, const char *line)
{
    pthread_mutex_lock(&q->mutex);

    if (q->count < QUEUE_CAPACITY)
    {
        strncpy(q->lines[q->tail], line, LINE_MAX_LEN - 1);
        q->lines[q->tail][LINE_MAX_LEN - 1] = '\0';
        q->tail = (q->tail + 1) % QUEUE_CAPACITY;
        q->count++;
        pthread_cond_signal(&q->cond);
    }

    pthread_mutex_unlock(&q->mutex);
}

static void Queue_Pop(CommandQueue *q, char *outLine)
{
    pthread_mutex_lock(&q->mutex);

    while (q->count == 0)
    {
        pthread_cond_wait(&q->cond, &q->mutex);
    }

    strcpy(outLine, q->lines[q->head]);
    q->head = (q->head + 1) % QUEUE_CAPACITY;
    q->count--;

    pthread_mutex_unlock(&q->mutex);
}

static void *StdinReaderThread(void *arg)
{
    (void)arg;
    char line[LINE_MAX_LEN];

    while (fgets(line, sizeof(line), stdin) != NULL)
    {
        line[strcspn(line, "\r\n")] = '\0';

        if (strcmp(line, "stop") == 0 || strcmp(line, "quit") == 0)
        {
            SearchStop();
        }

        Queue_Push(&g_queue, line);

        if (strcmp(line, "quit") == 0)
        {
            break;
        }
    }

    return NULL;
}

static double CalculateThinkTime(
    int wtime,
    int btime,
    int winc,
    int binc,
    int movestogo
);
/* ============================================================
   CONVERSION MOVE -> UCI
   ============================================================ */

static void PrintUCIMove(Move move)
{
    printf("%c%d%c%d",
           'a' + move.fromCol,
           8 - move.fromRow,
           'a' + move.toCol,
           8 - move.toRow);

    if (move.promotion != '\0')
    {
        char promotion = move.promotion;

        /*
         * UCI utilise toujours des lettres minuscules
         * pour la promotion.
         */
        if (promotion >= 'A' && promotion <= 'Z')
        {
            promotion += 'a' - 'A';
        }

        printf("%c", promotion);
    }
}


/* ============================================================
   CONVERSION UCI -> MOVE
   ============================================================ */

static int ParseUCIMove(
    const char *str,
    Move *move,
    int sideToMove)
{
    if (str == NULL || strlen(str) < 4)
        return 0;

    move->fromCol = str[0] - 'a';
    move->fromRow = 8 - (str[1] - '0');

    move->toCol = str[2] - 'a';
    move->toRow = 8 - (str[3] - '0');

    move->promotion = '\0';

    if (strlen(str) >= 5)
    {
        char p = str[4];


        if (p >= 'A' && p <= 'Z')
            p += 'a' - 'A'; // normaliser en minuscule d'abord

        move->promotion = (sideToMove == 0) ? (char)(p - 'a' + 'A') : p;
    }

    return 1;
}


/* ============================================================
   JOUER UN COUP UCI
   ============================================================ */

static int PlayUCIMove(const char *moveString)
{
    Move move;

    if (!ParseUCIMove(moveString, &move, position.sideToMove))
        return 0;

    MoveList legalMoves;

    GenerateLegalMoves(
        &position,
        &legalMoves
    );

    for (int i = 0; i < legalMoves.count; i++)
    {
        Move legal = legalMoves.moves[i];

        if (legal.fromRow == move.fromRow &&
            legal.fromCol == move.fromCol &&
            legal.toRow == move.toRow &&
            legal.toCol == move.toCol &&
            legal.promotion == move.promotion)
        {
            MakeMove(
                &position,
                legal
            );

            return 1;
        }
    }

    return 0;
}


/* ============================================================
   POSITION
   ============================================================ */

static void HandlePosition(char *command)
{
    char *token = strtok(command, " ");

    /*
     * token = "position"
     */

    token = strtok(NULL, " ");

    if (token == NULL)
        return;


    /*
     * --------------------------------------------------------
     * POSITION STARTPOS
     * --------------------------------------------------------
     */

    if (strcmp(token, "startpos") == 0)
    {
        InitBoard(&position);

        token = strtok(NULL, " ");
    }


    /*
     * --------------------------------------------------------
     * POSITION FEN
     * --------------------------------------------------------
     */

    else if (strcmp(token, "fen") == 0)
    {
        char fen[256] = "";

        /*
         * Une FEN contient 6 champs.
         */

        for (int i = 0; i < 6; i++)
        {
            token = strtok(NULL, " ");

            if (token == NULL)
                return;

            if (i > 0)
                strcat(fen, " ");

            strcat(fen, token);
        }

        InitPositionFromFEN(
            &position,
            fen
        );

        token = strtok(NULL, " ");
    }

    else
    {
        return;
    }


    /*
     * --------------------------------------------------------
     * MOVES
     * --------------------------------------------------------
     */

    if (token != NULL &&
        strcmp(token, "moves") == 0)
    {
        token = strtok(NULL, " ");

        while (token != NULL)
        {
            if (!PlayUCIMove(token))
            {
                /*
                 * Ne devrait plus arriver depuis le fix de la casse
                 * de promotion, mais si ca arrive quand meme, il
                 * vaut bien mieux le savoir (desync silencieuse =
                 * coups illegaux plus tard) que de continuer comme
                 * si de rien n'etait. stderr, jamais stdout : ca
                 * casserait le protocole UCI.
                 */
                fprintf(stderr, "[UCI] coup non reconnu, ignore : %s\n", token);
                fflush(stderr);
            }

            token = strtok(NULL, " ");
        }
    }
}


/* ============================================================
   GO
   ============================================================ */

/* ============================================================
   PROGRESSION DE LA RECHERCHE (callback pour "info")
   ============================================================ */

static void OnSearchProgress(int depth, int score, Move move)
{

    int uciScore = (position.sideToMove == 0) ? score : -score;

    printf("info depth %d score cp %d pv ", depth, uciScore);
    PrintUCIMove(move);
    printf("\n");
    fflush(stdout);
}


static void HandleGo(char *command)
{
    int depth = 30;

    int wtime = -1;
    int btime = -1;

    int winc = 0;
    int binc = 0;

    int movetime = -1;
    int movestogo = -1;

    char *token = strtok(command, " ");

    /*
     * "go"
     */
    token = strtok(NULL, " ");

    while (token != NULL)
    {
        if (strcmp(token, "depth") == 0)
        {
            token = strtok(NULL, " ");

            if (token != NULL)
                depth = atoi(token);
        }

        else if (strcmp(token, "movetime") == 0)
        {
            token = strtok(NULL, " ");

            if (token != NULL)
                movetime = atoi(token);
        }

        else if (strcmp(token, "wtime") == 0)
        {
            token = strtok(NULL, " ");

            if (token != NULL)
                wtime = atoi(token);
        }

        else if (strcmp(token, "btime") == 0)
        {
            token = strtok(NULL, " ");

            if (token != NULL)
                btime = atoi(token);
        }

        else if (strcmp(token, "winc") == 0)
        {
            token = strtok(NULL, " ");

            if (token != NULL)
                winc = atoi(token);
        }

        else if (strcmp(token, "binc") == 0)
        {
            token = strtok(NULL, " ");

            if (token != NULL)
                binc = atoi(token);
        }
        else if (strcmp(token, "movestogo") == 0)
        {
            token = strtok(NULL, " ");

            if (token != NULL)
                movestogo = atoi(token);
        }

        token = strtok(NULL, " ");
    }


    /*
     * --------------------------------------------------------
     * CALCUL DU TEMPS
     * --------------------------------------------------------
     */

    double timeSeconds;


    /*
     * go movetime
     */

    if (movetime >= 0)
    {
        timeSeconds =
            movetime / 1000.0;
    }

    /*
     * go wtime/btime
     */

    else if (wtime >= 0 && btime >= 0)
    {
        timeSeconds =
    CalculateThinkTime(
        wtime,
        btime,
        winc,
        binc,
        movestogo
    );
    }

    /*
     * go depth
     *
     * Si aucun temps n'est donné,
     * on garde une valeur de sécurité.
     */

    else
    {
        timeSeconds = 60.0;
    }


    /*
     * --------------------------------------------------------
     * RECHERCHE
     * --------------------------------------------------------
     */

    SearchResetStop();

    TT_ResetStats();

    SearchResult result =
        IterativeDeepening(
            &position,
            depth,
            timeSeconds,
            OnSearchProgress
        );


    /*
     * --------------------------------------------------------
     * RESULTAT
     * --------------------------------------------------------
     */

    printf("bestmove ");

    PrintUCIMove(result.move);

    printf("\n");

    fflush(stdout);
}


/* ============================================================
   UCI LOOP
   ============================================================ */

void UCI_Loop(void)
{
    Queue_Init(&g_queue);

    pthread_t readerThread;
    pthread_create(&readerThread, NULL, StdinReaderThread, NULL);

    char command[LINE_MAX_LEN];

    for (;;)
    {
        Queue_Pop(&g_queue, command);


        /*
         * ----------------------------------------------------
         * UCI
         * ----------------------------------------------------
         */

        if (strcmp(command, "uci") == 0)
        {
            printf("id name ChessBot\n");
            printf("id author Nimpso\n");

            printf("uciok\n");

            fflush(stdout);
        }


        /*
         * ----------------------------------------------------
         * ISREADY
         * ----------------------------------------------------
         */

        else if (strcmp(command, "isready") == 0)
        {
            printf("readyok\n");

            fflush(stdout);
        }


        /*
         * ----------------------------------------------------
         * UCINEWGAME
         * ----------------------------------------------------
         */

        else if (strcmp(command, "ucinewgame") == 0)
        {
            TT_Clear();

            InitBoard(&position);

            fflush(stdout);
        }


        /*
         * ----------------------------------------------------
         * POSITION
         * ----------------------------------------------------
         */

        else if (strncmp(command, "position", 8) == 0)
        {
            HandlePosition(command);
        }


        /*
         * ----------------------------------------------------
         * GO
         * ----------------------------------------------------
         */

        else if (strncmp(command, "go", 2) == 0)
        {
            HandleGo(command);
        }


        /*
         * ----------------------------------------------------
         * STOP
         * ----------------------------------------------------
         */

        else if (strcmp(command, "stop") == 0)
        {
            /*
             * Pour l'instant :
             * rien.
             *
             * On ajoutera ensuite un flag
             * de recherche interrompue.
             */
        }


        /*
         * ----------------------------------------------------
         * QUIT
         * ----------------------------------------------------
         */

        else if (strcmp(command, "quit") == 0)
        {
            break;
        }
    }

    pthread_join(readerThread, NULL);
}

static double CalculateThinkTime(
    int wtime,
    int btime,
    int winc,
    int binc,
    int movestogo)
{
    int timeRemaining;
    int increment;

    if (position.sideToMove == 0)
    {
        timeRemaining = wtime;
        increment = winc;
    }
    else
    {
        timeRemaining = btime;
        increment = binc;
    }

    /*
     * Nombre de coups restants avant le prochain contrôle
     * de temps.
     *
     * Si la GUI ne fournit pas movestogo, on utilise
     * une estimation conservatrice de 30 coups.
     */
    int moves = movestogo;

    if (moves <= 0)
        moves = 30;

    /*
     * Temps de base :
     *
     * temps restant / nombre de coups restants
     */
    double time = (double)timeRemaining / (double)moves / 1000.0;

    /*
     * On ajoute une partie de l'incrément.
     */
    time += (double)increment * 0.8 / 1000.0;

    /*
     * Marge de sécurité pour éviter de tomber à zéro.
     */
    double safety = 0.100;

    time -= safety;

    /*
     * Ne jamais utiliser plus de 25% du temps restant
     * sur un seul coup.
     */
    double maxTime =
        ((double)timeRemaining * 0.25) / 1000.0;

    if (time > maxTime)
        time = maxTime;

    /*
     * Temps minimum.
     */
    if (time < 0.01)
        time = 0.01;

    return time;
}