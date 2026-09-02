#include <vector>
#include <iostream>
#include "raylib.h"

constexpr int grid_wid = 80;
constexpr int original_x = 80;
constexpr int original_y = 180;
constexpr int window_width = 800;
constexpr int window_height = 1000;
constexpr int chess_width = 640;
constexpr int chess_height = 720;

int main(){
    //初始化窗口
    InitWindow(window_width, window_height, "Chinese Chess - Day 1");
    SetTargetFPS(60); //让窗口一秒绘制60次，限制while循环的速度

    //如果没有主动退出的话
    while (!WindowShouldClose()) {
        BeginDrawing();

        //底色和帧率显示
        ClearBackground(YELLOW);
        DrawFPS(10, 10);
        //绘制棋盘
        DrawRectangleLinesEx(
            Rectangle(original_x, original_y, chess_width, chess_height),
            4.0f, DARKGRAY
        );
        for (int i = 1; i <= 8; ++i) {
            DrawLineEx(
                Vector2{ original_x,(float)original_y + i * 80 },
                Vector2{ original_x + chess_width,(float)original_y + i * 80 },
                2.0f, DARKGRAY
            );
        }
        for (int i = 1; i <= 7; ++i) {
            DrawLineEx(
                Vector2{ (float)original_x + i * 80,original_y },
                Vector2{ (float)original_x + i * 80,original_y + chess_width / 2},
                2.0f, DARKGRAY
            );
        }
        for (int i = 1; i <= 7; ++i) {
            DrawLineEx(
                Vector2{ (float)original_x + i * 80,260.0f + chess_width / 2},
                Vector2{ (float)original_x + i * 80,original_y + chess_height },
                2.0f, DARKGRAY
            );
        }
        for (int i = 0; i < 2; ++i) {
            DrawLineEx(
                Vector2{ (float)original_x + 3 * 80,(float)original_y + i * 560},
                Vector2{ (float)original_x + 5 * 80,(float)original_y + i * 560 + 160 },
                2.0f, DARKGRAY
            );
        }
        for (int i = 0; i < 2; ++i) {
            DrawLineEx(
                Vector2{ (float)original_x + 5 * 80,(float)original_y + i * 560 },
                Vector2{ (float)original_x + 3 * 80,(float)original_y + i * 560 + 160 },
                2.0f, DARKGRAY
            );
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}