#include "board_painter.h"
#include "raylib.h"

void board_painter::draw_board() const{
    //绘制棋盘
    DrawRectangleLinesEx(
        Rectangle(original_x, original_y, chess_width, chess_height),
        4.0f, DARKGRAY
    );
    for (int i = 1; i <= 8; ++i) {
        DrawLineEx(
            Vector2{ original_x,(float)original_y + i * grid_wid },
            Vector2{ original_x + chess_width,(float)original_y + i * grid_wid },
            2.0f, DARKGRAY
        );
    }
    for (int i = 1; i <= 7; ++i) {
        DrawLineEx(
            Vector2{ (float)original_x + i * grid_wid,original_y },
            Vector2{ (float)original_x + i * grid_wid,original_y + chess_width / 2 },
            2.0f, DARKGRAY
        );
    }
    for (int i = 1; i <= 7; ++i) {
        DrawLineEx(
            Vector2{ (float)original_x + i * grid_wid,260.0f + chess_width / 2 },
            Vector2{ (float)original_x + i * grid_wid,original_y + chess_height },
            2.0f, DARKGRAY
        );
    }
    for (int i = 0; i < 2; ++i) {
        DrawLineEx(
            Vector2{ (float)original_x + 3 * grid_wid,(float)original_y + i * 560 },
            Vector2{ (float)original_x + 5 * grid_wid,(float)original_y + i * 560 + 160 },
            2.0f, DARKGRAY
        );
    }
    for (int i = 0; i < 2; ++i) {
        DrawLineEx(
            Vector2{ (float)original_x + 5 * grid_wid,(float)original_y + i * 560 },
            Vector2{ (float)original_x + 3 * grid_wid,(float)original_y + i * 560 + 160 },
            2.0f, DARKGRAY
        );
    }
}