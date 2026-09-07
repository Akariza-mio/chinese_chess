#include <cassert>
#include "raylib.h"
#include "board_painter.hpp"
#include "chess_board.hpp"
#include "rule_engine.hpp"


constexpr int window_width = 800;
constexpr int window_height = 1000;

void test() {
    chess_board board;
    rule_engine rules;
    board.clear();
    board.at({ 3,4 }) = piece(piece_type::Bing, piece_side::Black);
    board.at({ 4,4 }) = piece(piece_type::Bing, piece_side::Red);
    assert(
        rules.is_basic_move_valid(
            board,
            a_move{ {3, 4}, {4, 4} }
        )
    );
}
int main(){
    test();
    chess_board board;
    InitWindow(window_width, window_height, "Chinese Chess");
    SetTargetFPS(60);
    board_painter* bp = new board_painter();
    while (!WindowShouldClose()) {
        BeginDrawing();
        //底色和帧率显示
        ClearBackground(YELLOW);
        DrawFPS(10, 10);
        //绘制棋盘
        bp->draw_board();
        bp->draw_pieces(board);
        EndDrawing();
    }
    delete bp;
    CloseWindow();
}