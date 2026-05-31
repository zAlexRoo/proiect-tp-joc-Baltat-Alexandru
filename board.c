#include "board.h"
#include <stdio.h>
#include <string.h>

void init_board(char b[3][3]) {
    memset(b, '.', sizeof(char) * 9);
}

void print_board(char b[3][3]) {
    puts("\n  0 1 2");
    for (int r = 0; r < 3; r++)
        printf("%d %c %c %c\n", r, b[r][0], b[r][1], b[r][2]);
    putchar('\n');
}

int place(char b[3][3], int r, int c, char p) {
    if (r < 0 || r > 2 || c < 0 || c > 2) return 0;
    if (b[r][c] != '.') return 0;
    b[r][c] = p;
    return 1;
}

int check_win(char b[3][3], char p) {
    for (int i = 0; i < 3; i++) {
        if (b[i][0]==p && b[i][1]==p && b[i][2]==p) return 1;
        if (b[0][i]==p && b[1][i]==p && b[2][i]==p) return 1;
    }
    if (b[0][0]==p && b[1][1]==p && b[2][2]==p) return 1;
    if (b[0][2]==p && b[1][1]==p && b[2][0]==p) return 1;
    return 0;
}

int is_draw(char b[3][3]) {
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (b[i][j] == '.') return 0;
    return 1;
}
