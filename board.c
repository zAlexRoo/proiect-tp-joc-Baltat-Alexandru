#include "board.h"
#include <stdio.h>
#include <string.h>

void init_board(char b[6][6], int sz) {
    for (int i = 0; i < sz; i++)
        for (int j = 0; j < sz; j++)
            b[i][j] = '.';
}

void print_board(char b[6][6], int sz) {
    printf("\n  ");
    for (int i = 0; i < sz; i++) printf("%d ", i);
    puts("");
    for (int r = 0; r < sz; r++) {
        printf("%d ", r);
        for (int c = 0; c < sz; c++) {
            printf("%c ", b[r][c]);
        }
        putchar('\n');
    }
    putchar('\n');
}

int place(char b[6][6], int sz, int r, int c, char p) {
    if (r < 0 || r >= sz || c < 0 || c >= sz) return 0;
    if (b[r][c] != '.') return 0;
    b[r][c] = p;
    return 1;
}

int check_win(char b[6][6], int sz, char p) {
    for (int r = 0; r < sz; r++) {
        int win = 1;
        for (int c = 0; c < sz; c++) {
            if (b[r][c] != p) { win = 0; break; }
        }
        if (win) return 1;
    }
    for (int c = 0; c < sz; c++) {
        int win = 1;
        for (int r = 0; r < sz; r++) {
            if (b[r][c] != p) { win = 0; break; }
        }
        if (win) return 1;
    }
    int win_diag1 = 1;
    for (int i = 0; i < sz; i++) {
        if (b[i][i] != p) { win_diag1 = 0; break; }
    }
    if (win_diag1) return 1;

    int win_diag2 = 1;
    for (int i = 0; i < sz; i++) {
        if (b[i][sz - 1 - i] != p) { win_diag2 = 0; break; }
    }
    if (win_diag2) return 1;

    return 0;
}

int is_draw(char b[6][6], int sz) {
    for (int i = 0; i < sz; i++)
        for (int j = 0; j < sz; j++)
            if (b[i][j] == '.') return 0;
    return 1;
}