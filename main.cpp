#include <vector>
#include <iostream>
#include "raylib.h"

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
        beginDrawing();
        
        ClearBackground(RAYWHITE);
        DrawFPS(10, 10);
        DrawRectangleLinesEx(
            Rectangle(80, 180, chess_width, chess_height),
            4.0f, DARKGRAY
        );
        for (int i = 1; i <= 8; ++i) {
            DrawLineEx(
                Vector2{ 80.0f,180.0f + i * 80 },
                Vector2{ 80.0f + chess_width,180.0f + i * 80 },
                2.0f, DARKGRAY
            );
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}