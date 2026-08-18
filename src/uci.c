#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "uci.h"
#include "board.h"
#include "move.h"
#include "search.h"
#include "zobrist.h"
#include "tt.h"


static Position position;

static double CalculateThinkTime(
    int wtime,
    int btime,
    int winc,
    int binc
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
    Move *move)
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

        if (p >= 'a' && p <= 'z')
            p -= 'a' - 'A';

        move->promotion = p;
    }

    return 1;
}


/* ============================================================
   JOUER UN COUP UCI
   ============================================================ */

static int PlayUCIMove(const char *moveString)
{
    Move move;

    if (!ParseUCIMove(moveString, &move))
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
            PlayUCIMove(token);

            token = strtok(NULL, " ");
        }
    }
}


/* ============================================================
   GO
   ============================================================ */

static void HandleGo(char *command)
{
    int depth = 30;

    int wtime = -1;
    int btime = -1;

    int winc = 0;
    int binc = 0;

    int movetime = -1;

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
                binc
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
            timeSeconds
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
    char command[4096];

    while (fgets(command, sizeof(command), stdin))
    {
        /*
         * Enlever '\n'
         */
        command[strcspn(command, "\r\n")] = '\0';


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
}

static double CalculateThinkTime(
    int wtime,
    int btime,
    int winc,
    int binc)
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
     * On réserve une marge de sécurité.
     */

    double time =
        timeRemaining / 30.0;

    time += increment * 0.8 / 1000.0;

    /*
     * Ne jamais utiliser tout le temps.
     */

    double safety =
        0.050;

    time -= safety;

    if (time < 0.01)
        time = 0.01;

    return time;
}