#include "raylib.h"
#include "board.h"
#include "ai.h"
#include <stdio.h>
#include <string.h>

#define SCREEN_W   600
#define SCREEN_H   700
#define CELL_SIZE  160
#define GRID_OFF_X  60
#define GRID_OFF_Y 120
#define LINE_THICK   6

typedef enum { MENU, PLAYING, GAME_OVER } GameState;
typedef enum { PVP, VS_AI } GameMode;

static Color BG        = { 15,  15,  20, 255 };
static Color LINE_COL  = { 50,  50,  65, 255 };
static Color X_COL     = {220,  80,  80, 255 };
static Color O_COL     = { 70, 150, 255, 255 };
static Color WIN_COL   = {255, 210,  60, 255 };
static Color BTN_COL   = { 30,  30,  45, 255 };
static Color BTN_HOV   = { 50,  50,  75, 255 };
static Color TEXT_COL  = {230, 230, 240, 255 };
static Color DIM_COL   = { 90,  90, 110, 255 };

typedef struct { int r, c; } Cell;

char     board[3][3];
char     current;
GameState state;
GameMode  mode;
int      winner;          /* 0=none, 1=X, 2=O, 3=draw */
Cell     win_line[3];
int      score_x, score_o, score_draw;
float    cell_anim[3][3]; /* 0..1 scale animation */
float    win_anim;        /* 0..1 for win line */
bool     ai_thinking;
float    ai_timer;

void reset_board(void) {
    init_board(board);
    current  = 'X';
    winner   = 0;
    state    = PLAYING;
    ai_thinking = false;
    ai_timer    = 0;
    win_anim    = 0;
    memset(cell_anim, 0, sizeof(cell_anim));
}

void find_win_line(char p) {
    int lines[8][3][2] = {
        {{0,0},{0,1},{0,2}}, {{1,0},{1,1},{1,2}}, {{2,0},{2,1},{2,2}},
        {{0,0},{1,0},{2,0}}, {{0,1},{1,1},{2,1}}, {{0,2},{1,2},{2,2}},
        {{0,0},{1,1},{2,2}}, {{0,2},{1,1},{2,0}}
    };
    for (int i = 0; i < 8; i++) {
        if (board[lines[i][0][0]][lines[i][0][1]] == p &&
            board[lines[i][1][0]][lines[i][1][1]] == p &&
            board[lines[i][2][0]][lines[i][2][1]] == p) {
            for (int j = 0; j < 3; j++) {
                win_line[j].r = lines[i][j][0];
                win_line[j].c = lines[i][j][1];
            }
            return;
        }
    }
}

void draw_x(int cx, int cy, float scale, Color col) {
    float s = CELL_SIZE * 0.28f * scale;
    float t = LINE_THICK * 1.4f;
    DrawLineEx((Vector2){cx-s, cy-s}, (Vector2){cx+s, cy+s}, t, col);
    DrawLineEx((Vector2){cx+s, cy-s}, (Vector2){cx-s, cy+s}, t, col);
}

void draw_o(int cx, int cy, float scale, Color col) {
    float r = CELL_SIZE * 0.26f * scale;
    DrawRing((Vector2){cx, cy}, r - LINE_THICK*0.7f, r + LINE_THICK*0.7f,
             0, 360, 32, col);
}

bool draw_button(Rectangle rec, const char *label, int font_size) {
    bool hover = CheckCollisionPointRec(GetMousePosition(), rec);
    DrawRectangleRounded(rec, 0.2f, 8, hover ? BTN_HOV : BTN_COL);
    DrawRectangleRoundedLinesEx(rec, 0.2f, 8, 1.5f, hover ? O_COL : LINE_COL);
    int tw = MeasureText(label, font_size);
    DrawText(label, rec.x + (rec.width - tw)/2, rec.y + (rec.height - font_size)/2,
             font_size, hover ? TEXT_COL : DIM_COL);
    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Tic-Tac-Toe");
    SetTargetFPS(60);

    state = MENU;
    score_x = score_o = score_draw = 0;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        /* --- UPDATE --- */
        if (state == MENU) {
            Rectangle btn_pvp = { SCREEN_W/2 - 130, 280, 260, 56 };
            Rectangle btn_ai  = { SCREEN_W/2 - 130, 360, 260, 56 };
            if (draw_button(btn_pvp, "JUCATOR vs JUCATOR", 18)) {
                mode = PVP; reset_board();
            }
            if (draw_button(btn_ai, "JUCATOR vs AI", 18)) {
                mode = VS_AI; reset_board();
            }
        } else if (state == PLAYING) {
            /* animate pieces */
            for (int r = 0; r < 3; r++)
                for (int c = 0; c < 3; c++)
                    if (board[r][c] != '.' && cell_anim[r][c] < 1.0f) {
                        cell_anim[r][c] += dt * 6.0f;
                        if (cell_anim[r][c] > 1.0f) cell_anim[r][c] = 1.0f;
                    }

            /* AI move */
            if (mode == VS_AI && current == 'O' && !ai_thinking) {
                ai_thinking = true;
                ai_timer    = 0.4f;
            }
            if (ai_thinking) {
                ai_timer -= dt;
                if (ai_timer <= 0) {
                    ai_move(board, 'O');
                    ai_thinking = false;
                    /* find which cell changed */
                    for (int r = 0; r < 3; r++)
                        for (int c = 0; c < 3; c++)
                            if (board[r][c] == 'O' && cell_anim[r][c] == 0)
                                cell_anim[r][c] = 0.01f;

                    if (check_win(board, 'O')) {
                        find_win_line('O'); winner = 2; state = GAME_OVER; score_o++;
                    } else if (is_draw(board)) {
                        winner = 3; state = GAME_OVER; score_draw++;
                    } else {
                        current = 'X';
                    }
                }
            }

            /* Mouse click */
            if (!ai_thinking && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mp = GetMousePosition();
                for (int r = 0; r < 3; r++) {
                    for (int c = 0; c < 3; c++) {
                        Rectangle cell = {
                            GRID_OFF_X + c * CELL_SIZE,
                            GRID_OFF_Y + r * CELL_SIZE,
                            CELL_SIZE, CELL_SIZE
                        };
                        if (CheckCollisionPointRec(mp, cell) && board[r][c] == '.') {
                            place(board, r, c, current);
                            cell_anim[r][c] = 0.01f;
                            if (check_win(board, current)) {
                                find_win_line(current);
                                winner = (current == 'X') ? 1 : 2;
                                state  = GAME_OVER;
                                if (current == 'X') score_x++; else score_o++;
                            } else if (is_draw(board)) {
                                winner = 3; state = GAME_OVER; score_draw++;
                            } else {
                                current = (current == 'X') ? 'O' : 'X';
                                if (mode == VS_AI && current == 'O')
                                    ai_thinking = false;
                            }
                        }
                    }
                }
            }
        } else { /* GAME_OVER */
            win_anim += dt * 2.0f;
            if (win_anim > 1.0f) win_anim = 1.0f;
        }

        /* --- DRAW --- */
        BeginDrawing();
        ClearBackground(BG);

        if (state == MENU) {
            const char *title = "TIC-TAC-TOE";
            int tw = MeasureText(title, 52);
            DrawText(title, SCREEN_W/2 - tw/2, 120, 52, TEXT_COL);

            const char *sub = "Alege modul de joc";
            int sw = MeasureText(sub, 18);
            DrawText(sub, SCREEN_W/2 - sw/2, 210, 18, DIM_COL);

            Rectangle btn_pvp = { SCREEN_W/2 - 130, 280, 260, 56 };
            Rectangle btn_ai  = { SCREEN_W/2 - 130, 360, 260, 56 };
            draw_button(btn_pvp, "JUCATOR vs JUCATOR", 18);
            draw_button(btn_ai,  "JUCATOR vs AI",      18);
        } else {
            /* Score bar */
            char sx[16], so[16], sd[16];
            sprintf(sx, "%d", score_x);
            sprintf(so, "%d", score_o);
            sprintf(sd, "%d", score_draw);

            DrawText("X",  90, 20, 28, X_COL);
            DrawText(sx,  115, 22, 22, TEXT_COL);
            DrawText("EGAL", SCREEN_W/2 - 28, 22, 18, DIM_COL);
            DrawText(sd,  SCREEN_W/2 + 28, 22, 22, TEXT_COL);
            DrawText(so, SCREEN_W - 130, 22, 22, TEXT_COL);
            DrawText("O",  SCREEN_W - 105, 20, 28, O_COL);

            /* Status */
            const char *status;
            if (state == PLAYING) {
                if (ai_thinking)           status = "AI se gandeste...";
                else if (current == 'X')   status = "Randul lui X";
                else                       status = "Randul lui O";
            } else {
                if (winner == 1)      status = "X a castigat!";
                else if (winner == 2) status = "O a castigat!";
                else                  status = "Egal!";
            }
            int stw = MeasureText(status, 22);
            DrawText(status, SCREEN_W/2 - stw/2, 68, 22,
                     winner == 1 ? X_COL : winner == 2 ? O_COL : TEXT_COL);

            /* Grid lines */
            for (int i = 1; i < 3; i++) {
                DrawLineEx(
                    (Vector2){GRID_OFF_X + i*CELL_SIZE, GRID_OFF_Y},
                    (Vector2){GRID_OFF_X + i*CELL_SIZE, GRID_OFF_Y + 3*CELL_SIZE},
                    LINE_THICK, LINE_COL);
                DrawLineEx(
                    (Vector2){GRID_OFF_X, GRID_OFF_Y + i*CELL_SIZE},
                    (Vector2){GRID_OFF_X + 3*CELL_SIZE, GRID_OFF_Y + i*CELL_SIZE},
                    LINE_THICK, LINE_COL);
            }

            /* Pieces */
            for (int r = 0; r < 3; r++) {
                for (int c = 0; c < 3; c++) {
                    int cx = GRID_OFF_X + c*CELL_SIZE + CELL_SIZE/2;
                    int cy = GRID_OFF_Y + r*CELL_SIZE + CELL_SIZE/2;
                    float sc = cell_anim[r][c];
                    /* bounce easing */
                    float es = sc < 0.5f ? 2*sc*sc : 1 - (-2*sc+2)*(-2*sc+2)/2;

                    if (board[r][c] == 'X') draw_x(cx, cy, es, X_COL);
                    else if (board[r][c] == 'O') draw_o(cx, cy, es, O_COL);
                }
            }

            /* Win line */
            if (state == GAME_OVER && winner != 3) {
                int ax = GRID_OFF_X + win_line[0].c*CELL_SIZE + CELL_SIZE/2;
                int ay = GRID_OFF_Y + win_line[0].r*CELL_SIZE + CELL_SIZE/2;
                int bx = GRID_OFF_X + win_line[2].c*CELL_SIZE + CELL_SIZE/2;
                int by = GRID_OFF_Y + win_line[2].r*CELL_SIZE + CELL_SIZE/2;
                int mx = ax + (int)((bx-ax) * win_anim);
                int my = ay + (int)((by-ay) * win_anim);
                DrawLineEx((Vector2){ax,ay}, (Vector2){mx,my}, LINE_THICK+2, WIN_COL);
            }

            /* Buttons */
            Rectangle btn_new  = { 60,  620, 180, 48 };
            Rectangle btn_menu = { 360, 620, 180, 48 };
            if (draw_button(btn_new,  "JOC NOU",  17)) reset_board();
            if (draw_button(btn_menu, "MENIU",    17)) { state = MENU; }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}