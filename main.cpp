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
    board.at({ 0,4 }) = piece(piece_type::General, piece_side::Black);
    board.at({ 9,5 }) = piece(piece_type::General, piece_side::Red);
    board.at({ 1,2 }) = piece(piece_type::Ma, piece_side::Red);
    assert(rules.is_in_check(board,piece_side::Black));
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