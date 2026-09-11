#include <optional>
#include "raylib.h"
#include "board_painter.hpp"
#include "chess_board.hpp"
#include "rule_engine.hpp"
constexpr int window_width = 800;
constexpr int window_height = 1000;

int main(){
    chess_board board;
    InitWindow(window_width, window_height, "Chinese Chess");
    SetTargetFPS(60);
    board_painter* bp = new board_painter();
    rule_engine* rule = new rule_engine();
    std::optional<pos> selected_pos = std::nullopt;
    piece_side now_turn = piece_side::Red;
    while (!WindowShouldClose()) {
        BeginDrawing();
        //底色和帧率显示
        ClearBackground(YELLOW);
        DrawFPS(10, 10);
        //绘制棋盘
        bp->draw_board();
        bp->draw_pieces(board);
        if (selected_pos.has_value()) {
            bp->draw_selected_sign(*selected_pos);
        }
        EndDrawing();
    }
    delete bp;
    delete rule;
    CloseWindow();
}
