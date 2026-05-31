#include "ai.h"
#include "board.h"
#include <limits.h>
#include <stdlib.h>

static int minimax(char b[6][6], int is_max) {
    if (check_win(b, 3, 'O')) return  10;
    if (check_win(b, 3, 'X')) return -10;
    if (is_draw(b, 3))        return   0;

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

Cell ai_move(char b[6][6], int sz, char player, int diff, int is_infinite) {
    Cell choice = {-1, -1};
    int available_r[36];
    int available_c[36];
    int count = 0;

    for (int r = 0; r < sz; r++) {
        for (int c = 0; c < sz; c++) {
            if (b[r][c] == '.') {
                available_r[count] = r;
                available_c[count] = c;
                count++;
            }
        }
    }

    if (count == 0) return choice;

    // 1. EASY
    if (diff == 0) {
        int idx = rand() % count;
        choice.r = available_r[idx];
        choice.c = available_c[idx];
        return choice;
    }

    // 2. MEDIUM
    if (diff == 1) {
        if (rand() % 2 == 0) {
            int idx = rand() % count;
            choice.r = available_r[idx];
            choice.c = available_c[idx];
            return choice;
        }
    }

    // 3. HARD sau Inteligenta de calcul (Foloseste Minimax doar daca e Clasic 3x3)
    if (!is_infinite && sz == 3) {
        int best = INT_MIN;
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                if (b[r][c] != '.') continue;
                b[r][c] = player;
                int s = minimax(b, 0);
                b[r][c] = '.';
                if (s > best) { best = s; choice.r = r; choice.c = c; }
            }
        }
        return choice;
    }

    // Euristica rapida si inteligenta pentru Modul Infinit sau Dimensiuni Mari
    char opponent = (player == 'X') ? 'O' : 'X';

    // Pasul 1: Poate castiga AI-ul tura asta?
    for (int r = 0; r < sz; r++) {
        for (int c = 0; c < sz; c++) {
            if (b[r][c] == '.') {
                b[r][c] = player;
                int win = check_win(b, sz, player);
                b[r][c] = '.';
                if (win) { choice.r = r; choice.c = c; return choice; }
            }
        }
    }

    // Pasul 2: Trebuie sa blocheze jucatorul?
    for (int r = 0; r < sz; r++) {
        for (int c = 0; c < sz; c++) {
            if (b[r][c] == '.') {
                b[r][c] = opponent;
                int win = check_win(b, sz, opponent);
                b[r][c] = '.';
                if (win) { choice.r = r; choice.c = c; return choice; }
            }
        }
    }

    // Pasul 3: Prioritate pe centru
    int center = sz / 2;
    if (b[center][center] == '.') {
        choice.r = center; choice.c = center;
        return choice;
    }

    // Pasul 4: Mutare random sigura
    int idx = rand() % count;
    choice.r = available_r[idx];
    choice.c = available_c[idx];
    return choice;
}