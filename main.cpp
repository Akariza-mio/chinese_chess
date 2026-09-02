#include "raylib.h"
#include "board_painter.h"

constexpr int window_width = 800;
constexpr int window_height = 1000;
int main(){
    InitWindow(window_width, window_height, "Chinese Chess");
    SetTargetFPS(60);
    board_painter* bp = new board_painter();
    while (!WindowShouldClose()) {
        BeginDrawing();
        //底色和帧率显示
        ClearBackground(YELLOW);
        DrawFPS(10, 10);
        bp->draw_board();
        EndDrawing();
    }
    delete bp;
    CloseWindow();
}