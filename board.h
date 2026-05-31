#ifndef BOARD_H
#define BOARD_H

void init_board(char b[3][3]);
void print_board(char b[3][3]);
int  place(char b[3][3], int row, int col, char player);
int  check_win(char b[3][3], char player);
int  is_draw(char b[3][3]);

#endif
