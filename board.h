#ifndef BOARD_H
#define BOARD_H

/* Structura Cell mutata aici pentru a fi accesibila global de catre main si ai */
typedef struct { int r, c; } Cell;

void init_board(char b[6][6], int sz);
void print_board(char b[6][6], int sz);
int  place(char b[6][6], int sz, int row, int col, char player);
int  check_win(char b[6][6], int sz, char player);
int  is_draw(char b[6][6], int sz);

#endif