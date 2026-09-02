#include "board_painter.hpp"
#include "raylib.h"

board_painter::board_painter() {
    constexpr int chuhehanjie[] = {
        0x695A, // 楚
        0x6CB3, // 河
        0x6C49, // 汉
        0x754C  // 界
    };
    chinese_font = LoadFontEx("assets/fonts/NotoSansCJKsc-Regular.otf", 48, chuhehanjie, 4);
}
board_painter::~board_painter() {
    UnloadFont(chinese_font);
}
//画炮兵位的记号
void board_painter::draw_jin(int x,int y,int side) const {
    if (side == 0) {
        for (int i = 0; i < 2; ++i) {
            DrawLineEx(
                Vector2{ (float)x - 17 ,(float)y - 5 + i * 10 },
                Vector2{ (float)x - 7 ,(float)y - 5 + i * 10 },
                2.0f, DARKGRAY
            );
        }
        for (int i = 0; i < 2; ++i) {
            DrawLineEx(
                Vector2{ (float)x - 7 ,(float)y - 15 + i * 20 },
                Vector2{ (float)x - 7 ,(float)y - 5 + i * 20 },
                2.0f, DARKGRAY
            );
        }
        return;
    }
    if (side == 1) {
        for (int i = 0; i < 2; ++i) {
            DrawLineEx(
                Vector2{ (float)x + 17 ,(float)y - 5 + i * 10 },
                Vector2{ (float)x + 7 ,(float)y - 5 + i * 10 },
                2.0f, DARKGRAY
            );
        }
        for (int i = 0; i < 2; ++i) {
            DrawLineEx(
                Vector2{ (float)x + 7 ,(float)y - 15 + i * 20 },
                Vector2{ (float)x + 7 ,(float)y - 5 + i * 20 },
                2.0f, DARKGRAY
            );
        }
        return;
    }
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            DrawLineEx(
                Vector2{ (float)x - 15 + j * 20,(float)y - 5 + i * 10},
                Vector2{ (float)x - 5 + j * 20,(float)y - 5 + i * 10},
                2.0f, DARKGRAY
            );
        }
    }
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            DrawLineEx(
                Vector2{ (float)x - 5 + j * 10,(float)y - 15 + i * 20 },
                Vector2{ (float)x - 5 + j * 10,(float)y - 5 + i * 20 },
                2.0f, DARKGRAY
            );
        }
    }
}
//绘制棋盘
void board_painter::draw_board() const{
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
    DrawTextEx(chinese_font, "楚河", Vector2{ 200.0f, 515.0f }, 48.0f, 4.0f, DARKGRAY);
    DrawTextEx(chinese_font, "汉界", Vector2{ 520.0f, 515.0f }, 48.0f, 4.0f, DARKGRAY);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            draw_jin(original_x + grid_wid + j * grid_wid * 6,
                original_y + 2 * grid_wid + i * grid_wid * 5,2);
        }
    }
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            draw_jin(original_x + 2 * grid_wid + j * grid_wid * 2,
                original_y + 3 * grid_wid + i * grid_wid * 3,2);
        }
    }
    draw_jin(original_x, original_y + 3 * grid_wid, 1);
    draw_jin(original_x, original_y + 6 * grid_wid, 1);
    draw_jin(original_x + chess_width, original_y + 3 * grid_wid, 0);
    draw_jin(original_x + chess_width, original_y + 6 * grid_wid, 0);
}