#include "ai.h"
#include "board.h"
#include <limits.h>

static int minimax(char b[3][3], int is_max) {
    if (check_win(b, 'O')) return  10;
    if (check_win(b, 'X')) return -10;
    if (is_draw(b))        return   0;

    int best = is_max ? INT_MIN : INT_MAX;
    char p = is_max ? 'O' : 'X';

    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            if (b[r][c] != '.') continue;
            b[r][c] = p;
            int score = minimax(b, !is_max);
            b[r][c] = '.';
            if (is_max) { if (score > best) best = score; }
            else        { if (score < best) best = score; }
        }
    }
    return best;
}

void ai_move(char b[3][3], char player) {
    int best = INT_MIN, br = -1, bc = -1;
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            if (b[r][c] != '.') continue;
            b[r][c] = player;
            int s = minimax(b, 0);
            b[r][c] = '.';
            if (s > best) { best = s; br = r; bc = c; }
        }
    }
    b[br][bc] = player;
}
