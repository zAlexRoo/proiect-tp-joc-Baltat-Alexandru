#include "raylib.h"
#include "board.h"
#include "ai.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SCREEN_W   600
#define SCREEN_H   750
#define GRID_SIZE_MAX 480
#define GRID_OFF_X  60
#define GRID_OFF_Y 120
#define LINE_THICK   6

typedef enum { MENU, MODE_MENU, AI_MENU, GRID_MENU, PLAYING, GAME_OVER } GameState;
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

char     board[6][6];
int      grid_size = 3;   
int      cell_size = 160; 
char     current;
GameState state;
GameMode  mode;
int      ai_difficulty;   /* 0 = Easy, 1 = Medium, 2 = Hard */
int      winner;          /* 0=none, 1=X, 2=O, 3=draw */

/* --- VARIABILE AUDIO --- */
Sound    fx_click;
Sound    fx_win;
Sound    fx_lose;

/* --- VARIABILE PENTRU MODUL INFINIT --- */
bool     is_infinite = false;
int      move_history_r[2][6]; 
int      move_history_c[2][6]; 
int      move_count[2];        

typedef struct { int r1, c1, r2, c2; } WinLineCoords;
WinLineCoords wl;

int      score_x, score_o, score_draw;
float    cell_anim[6][6]; 
float    win_anim;        
bool     ai_thinking;
float    ai_timer;

void reset_board(void) {
    init_board(board, grid_size);
    cell_size = GRID_SIZE_MAX / grid_size;
    current  = 'X';
    winner   = 0;
    state    = PLAYING;
    ai_thinking = false;
    ai_timer    = 0;
    win_anim    = 0;
    
    move_count[0] = 0;
    move_count[1] = 0;
    memset(move_history_r, 0, sizeof(move_history_r));
    memset(move_history_c, 0, sizeof(move_history_c));
    memset(cell_anim, 0, sizeof(cell_anim));
}

void execute_move(int r, int c, char p) {
    int p_idx = (p == 'X') ? 0 : 1;

    if (is_infinite && move_count[p_idx] == grid_size) {
        int old_r = move_history_r[p_idx][0];
        int old_c = move_history_c[p_idx][0];
        
        board[old_r][old_c] = '.';
        cell_anim[old_r][old_c] = 0.0f;

        for (int i = 0; i < move_count[p_idx] - 1; i++) {
            move_history_r[p_idx][i] = move_history_r[p_idx][i + 1];
            move_history_c[p_idx][i] = move_history_c[p_idx][i + 1];
        }
        move_count[p_idx]--;
    }

    board[r][c] = p;
    cell_anim[r][c] = 0.01f;

    /* Declanșăm sunetul la fiecare piesă pusă */
    PlaySound(fx_click);

    if (is_infinite) {
        move_history_r[p_idx][move_count[p_idx]] = r;
        move_history_c[p_idx][move_count[p_idx]] = c;
        move_count[p_idx]++;
    }
}

void find_win_line_coords(char p) {
    for (int r = 0; r < grid_size; r++) {
        int win = 1;
        for (int c = 0; c < grid_size; c++) if (board[r][c] != p) { win = 0; break; }
        if (win) { wl = (WinLineCoords){r, 0, r, grid_size-1}; return; }
    }
    for (int c = 0; c < grid_size; c++) {
        int win = 1;
        for (int r = 0; r < grid_size; r++) if (board[r][c] != p) { win = 0; break; }
        if (win) { wl = (WinLineCoords){0, c, grid_size-1, c}; return; }
    }
    int win_d1 = 1;
    for (int i = 0; i < grid_size; i++) if (board[i][i] != p) { win_d1 = 0; break; }
    if (win_d1) { wl = (WinLineCoords){0, 0, grid_size-1, grid_size-1}; return; }

    int win_d2 = 1;
    for (int i = 0; i < grid_size; i++) if (board[i][grid_size-1-i] != p) { win_d2 = 0; break; }
    if (win_d2) { wl = (WinLineCoords){0, grid_size-1, grid_size-1, 0}; return; }
}

void draw_x(int cx, int cy, float scale, Color col) {
    float s = cell_size * 0.28f * scale;
    float t = LINE_THICK * (160.0f / cell_size) * 0.8f;
    if (t < 3) t = 3;
    DrawLineEx((Vector2){cx-s, cy-s}, (Vector2){cx+s, cy+s}, t, col);
    DrawLineEx((Vector2){cx+s, cy-s}, (Vector2){cx-s, cy+s}, t, col);
}

void draw_o(int cx, int cy, float scale, Color col) {
    float r = cell_size * 0.26f * scale;
    float t = LINE_THICK * (160.0f / cell_size) * 0.6f;
    if (t < 3) t = 3;
    DrawRing((Vector2){cx, cy}, r - t, r + t, 0, 360, 32, col);
}

bool draw_button(Rectangle rec, const char *label, int font_size, Color hover_col) {
    bool hover = CheckCollisionPointRec(GetMousePosition(), rec);
    DrawRectangleRounded(rec, 0.2f, 8, hover ? hover_col : BTN_COL);
    DrawRectangleRoundedLinesEx(rec, 0.2f, 8, 1.5f, hover ? hover_col : LINE_COL);
    int tw = MeasureText(label, font_size);
    DrawText(label, rec.x + (rec.width - tw)/2, rec.y + (rec.height - font_size)/2,
             font_size, hover ? TEXT_COL : DIM_COL);
    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

int main(void) {
    srand(time(NULL));
    InitWindow(SCREEN_W, SCREEN_H, "Tic-Tac-Toe");
    
    /* --- INITIALIZARE MOTOR AUDIO --- */
    InitAudioDevice(); 
    
    SetTargetFPS(60);

    /* --- INCARCARE SUNETE --- */
    fx_click = LoadSound("click.wav");
    fx_win   = LoadSound("victory.wav"); 
    fx_lose  = LoadSound("lose.wav");

    /* --- AJUSTARI AUDIO PENTRU CLICK --- */
    SetSoundVolume(fx_click, 0.5f); // Da volumul la 50%
    SetSoundPitch(fx_click, 1.5f);  // Face sunetul mai rapid si mai ascutit

    state = MENU;
    score_x = score_o = score_draw = 0;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        /* Animatii */
        for (int r = 0; r < grid_size; r++) {
            for (int c = 0; c < grid_size; c++) {
                if (board[r][c] != '.' && cell_anim[r][c] < 1.0f) {
                    cell_anim[r][c] += dt * 6.0f;
                    if (cell_anim[r][c] > 1.0f) cell_anim[r][c] = 1.0f;
                }
            }
        }

        /* --- UPDATE --- */
        if (state == MENU) {
            Rectangle btn_classic  = { SCREEN_W/2 - 130, 280, 260, 56 };
            Rectangle btn_infinite = { SCREEN_W/2 - 130, 360, 260, 56 };
            if (draw_button(btn_classic, "MOD CLASIC", 18, BTN_HOV)) {
                is_infinite = false; state = MODE_MENU;
            }
            if (draw_button(btn_infinite, "INFINITE MODE", 18, (Color){ 99, 102, 241, 255 })) {
                is_infinite = true; state = MODE_MENU;
            }
        } else if (state == MODE_MENU) {
            Rectangle btn_pvp  = { SCREEN_W/2 - 130, 280, 260, 56 };
            Rectangle btn_ai   = { SCREEN_W/2 - 130, 360, 260, 56 };
            Rectangle btn_back = { SCREEN_W/2 - 130, 450, 260, 44 };
            if (draw_button(btn_pvp, "JUCATOR vs JUCATOR", 18, BTN_HOV)) {
                mode = PVP; state = GRID_MENU;
            }
            if (draw_button(btn_ai, "JUCATOR vs AI", 18, BTN_HOV)) {
                mode = VS_AI; state = AI_MENU;
            }
            if (draw_button(btn_back, "INAPOI", 16, BTN_HOV)) state = MENU;

        } else if (state == AI_MENU) {
            Rectangle btn_easy = { SCREEN_W/2 - 130, 260, 260, 56 };
            Rectangle btn_med  = { SCREEN_W/2 - 130, 340, 260, 56 };
            Rectangle btn_hard = { SCREEN_W/2 - 130, 420, 260, 56 };
            Rectangle btn_back = { SCREEN_W/2 - 130, 510, 260, 44 };

            if (draw_button(btn_easy, "EASY", 18, (Color){ 46, 184, 114, 255 })) {
                ai_difficulty = 0; state = GRID_MENU;
            }
            if (draw_button(btn_med, "MEDIUM", 18, (Color){ 234, 179, 8, 255 })) {
                ai_difficulty = 1; state = GRID_MENU;
            }
            if (draw_button(btn_hard, "HARD", 18, (Color){ 239, 68, 68, 255 })) {
                ai_difficulty = 2; state = GRID_MENU;
            }
            if (draw_button(btn_back, "INAPOI", 16, BTN_HOV)) state = MODE_MENU;

        } else if (state == GRID_MENU) {
            Rectangle b3 = { SCREEN_W/2 - 130, 240, 260, 50 };
            Rectangle b4 = { SCREEN_W/2 - 130, 310, 260, 50 };
            Rectangle b5 = { SCREEN_W/2 - 130, 380, 260, 50 };
            Rectangle b6 = { SCREEN_W/2 - 130, 450, 260, 50 };
            Rectangle btn_back = { SCREEN_W/2 - 130, 530, 260, 44 };

            Color hgc = { 99, 102, 241, 255 };

            if (draw_button(b3, "TABLA 3 x 3", 18, hgc)) { grid_size = 3; reset_board(); }
            if (draw_button(b4, "TABLA 4 x 4", 18, hgc)) { grid_size = 4; reset_board(); }
            if (draw_button(b5, "TABLA 5 x 5", 18, hgc)) { grid_size = 5; reset_board(); }
            if (draw_button(b6, "TABLA 6 x 6", 18, hgc)) { grid_size = 6; reset_board(); }
            if (draw_button(btn_back, "INAPOI", 16, BTN_HOV)) {
                state = (mode == VS_AI) ? AI_MENU : MODE_MENU;
            }

        } else if (state == PLAYING) {
            /* AI Turn */
            if (mode == VS_AI && current == 'O' && !ai_thinking) {
                ai_thinking = true;
                ai_timer    = 0.4f;
            }
            if (ai_thinking) {
                ai_timer -= dt;
                if (ai_timer <= 0) {
                    Cell ai_choice = ai_move(board, grid_size, 'O', ai_difficulty, is_infinite);
                    ai_thinking = false;
                    
                    if (ai_choice.r != -1 && ai_choice.c != -1) {
                        execute_move(ai_choice.r, ai_choice.c, 'O');

                        if (check_win(board, grid_size, 'O')) {
                            find_win_line_coords('O'); winner = 2; state = GAME_OVER; score_o++;
                            PlaySound(fx_lose);
                        } else if (is_draw(board, grid_size)) {
                            winner = 3; state = GAME_OVER; score_draw++;
                        } else {
                            current = 'X';
                        }
                    }
                }
            }

            /* Click Jucator */
            if (!ai_thinking && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mp = GetMousePosition();
                for (int r = 0; r < grid_size; r++) {
                    for (int c = 0; c < grid_size; c++) {
                        Rectangle cell = {
                            GRID_OFF_X + c * cell_size,
                            GRID_OFF_Y + r * cell_size,
                            cell_size, cell_size
                        };
                        if (CheckCollisionPointRec(mp, cell) && board[r][c] == '.') {
                            execute_move(r, c, current);

                            if (check_win(board, grid_size, current)) {
                                find_win_line_coords(current);
                                winner = (current == 'X') ? 1 : 2;
                                state  = GAME_OVER;
                                if (current == 'X') score_x++; else score_o++;
                                PlaySound(fx_win);
                            } else if (is_draw(board, grid_size)) {
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

            Rectangle btn_classic  = { SCREEN_W/2 - 130, 280, 260, 56 };
            Rectangle btn_infinite = { SCREEN_W/2 - 130, 360, 260, 56 };
            draw_button(btn_classic,  "MOD CLASIC", 18, BTN_HOV);
            draw_button(btn_infinite, "INFINITE MODE", 18, (Color){ 99, 102, 241, 255 });
        } else if (state == MODE_MENU) {
            const char *title = is_infinite ? "INFINITE MODE" : "MOD CLASIC";
            int tw = MeasureText(title, 46);
            DrawText(title, SCREEN_W/2 - tw/2, 120, 46, is_infinite ? (Color){ 99, 102, 241, 255 } : TEXT_COL);

            Rectangle btn_pvp  = { SCREEN_W/2 - 130, 280, 260, 56 };
            Rectangle btn_ai   = { SCREEN_W/2 - 130, 360, 260, 56 };
            Rectangle btn_back = { SCREEN_W/2 - 130, 450, 260, 44 };
            draw_button(btn_pvp,  "JUCATOR vs JUCATOR", 18, BTN_HOV);
            draw_button(btn_ai,   "JUCATOR vs AI",      18, BTN_HOV);
            draw_button(btn_back, "INAPOI", 16, BTN_HOV);
        } else if (state == AI_MENU) {
            const char *title = "DIFICULTATE AI";
            int tw = MeasureText(title, 46);
            DrawText(title, SCREEN_W/2 - tw/2, 120, 46, TEXT_COL);

            Rectangle btn_easy = { SCREEN_W/2 - 130, 260, 260, 56 };
            Rectangle btn_med  = { SCREEN_W/2 - 130, 340, 260, 56 };
            Rectangle btn_hard = { SCREEN_W/2 - 130, 420, 260, 56 };
            Rectangle btn_back = { SCREEN_W/2 - 130, 510, 260, 44 };
            draw_button(btn_easy, "EASY", 18, (Color){ 46, 184, 114, 255 });
            draw_button(btn_med, "MEDIUM", 18, (Color){ 234, 179, 8, 255 });
            draw_button(btn_hard, "HARD", 18, (Color){ 239, 68, 68, 255 });
            draw_button(btn_back, "INAPOI", 16, BTN_HOV);
        } else if (state == GRID_MENU) {
            const char *title = "DIMENSIUNE TABLA";
            int tw = MeasureText(title, 42);
            DrawText(title, SCREEN_W/2 - tw/2, 120, 42, TEXT_COL);

            Rectangle b3 = { SCREEN_W/2 - 130, 240, 260, 50 };
            Rectangle b4 = { SCREEN_W/2 - 130, 310, 260, 50 };
            Rectangle b5 = { SCREEN_W/2 - 130, 380, 260, 50 };
            Rectangle b6 = { SCREEN_W/2 - 130, 450, 260, 50 };
            Rectangle btn_back = { SCREEN_W/2 - 130, 530, 260, 44 };
            Color hgc = { 99, 102, 241, 255 };
            draw_button(b3, "TABLA 3 x 3", 18, hgc);
            draw_button(b4, "TABLA 4 x 4", 18, hgc);
            draw_button(b5, "TABLA 5 x 5", 18, hgc);
            draw_button(b6, "TABLA 6 x 6", 18, hgc);
            draw_button(btn_back, "INAPOI", 16, BTN_HOV);
        } else {
            /* Barele de scor */
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

            /* Text status */
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

            /* Linii Grid */
            for (int i = 1; i < grid_size; i++) {
                DrawLineEx(
                    (Vector2){GRID_OFF_X + i * cell_size, GRID_OFF_Y},
                    (Vector2){GRID_OFF_X + i * cell_size, GRID_OFF_Y + GRID_SIZE_MAX},
                    LINE_THICK, LINE_COL);
                DrawLineEx(
                    (Vector2){GRID_OFF_X, GRID_OFF_Y + i * cell_size},
                    (Vector2){GRID_OFF_X + GRID_SIZE_MAX, GRID_OFF_Y + i * cell_size},
                    LINE_THICK, LINE_COL);
            }

            /* Afisare piese X si O */
            for (int r = 0; r < grid_size; r++) {
                for (int c = 0; c < grid_size; c++) {
                    if (board[r][c] != '.') {
                        int cx = GRID_OFF_X + c * cell_size + cell_size / 2;
                        int cy = GRID_OFF_Y + r * cell_size + cell_size / 2;
                        float sc = cell_anim[r][c];
                        float es = sc < 0.5f ? 2*sc*sc : 1 - (-2*sc+2)*(-2*sc+2)/2;

                        Color piece_color = (board[r][c] == 'X') ? X_COL : O_COL;

                        if (is_infinite && state == PLAYING) {
                            int p_idx = (board[r][c] == 'X') ? 0 : 1;
                            if (move_count[p_idx] == grid_size && 
                                move_history_r[p_idx][0] == r && 
                                move_history_c[p_idx][0] == c) {
                                
                                float alpha = 0.25f + 0.45f * fabsf(sinf(GetTime() * 6.0f));
                                piece_color.a = (unsigned char)(alpha * 255);
                            }
                        }

                        if (board[r][c] == 'X') draw_x(cx, cy, es, piece_color);
                        else if (board[r][c] == 'O') draw_o(cx, cy, es, piece_color);
                    }
                }
            }

            /* Linie Victorie */
            if (state == GAME_OVER && winner != 3) {
                int ax = GRID_OFF_X + wl.c1 * cell_size + cell_size / 2;
                int ay = GRID_OFF_Y + wl.r1 * cell_size + cell_size / 2;
                int bx = GRID_OFF_X + wl.c2 * cell_size + cell_size / 2;
                int by = GRID_OFF_Y + wl.r2 * cell_size + cell_size / 2;
                int mx = ax + (int)((bx - ax) * win_anim);
                int my = ay + (int)((by - ay) * win_anim);
                DrawLineEx((Vector2){ax, ay}, (Vector2){mx, my}, LINE_THICK + 2, WIN_COL);
            }

            /* Butoane Meniu In-Game */
            Rectangle btn_new  = { 60,  660, 180, 48 };
            Rectangle btn_menu = { 360, 660, 180, 48 };
            if (draw_button(btn_new,  "JOC NOU",  17, BTN_HOV)) reset_board();
            if (draw_button(btn_menu, "MENIU",    17, BTN_HOV)) state = MENU;
        }

        EndDrawing();
    }

    /* --- CURATARE AUDIO SI FINISH --- */
    UnloadSound(fx_click);
    UnloadSound(fx_win);
    UnloadSound(fx_lose);
    CloseAudioDevice(); 

    CloseWindow();
    return 0;
}