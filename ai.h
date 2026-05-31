#ifndef AI_H
#define AI_H

#include "board.h"

/* Functia returneaza acum o structura Cell cu coordonatele alese de AI */
Cell ai_move(char b[6][6], int sz, char player, int diff, int is_infinite);

#endif