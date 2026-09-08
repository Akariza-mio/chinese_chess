#include <cassert>
#include "raylib.h"
#include "board_painter.hpp"
#include "chess_board.hpp"
#include "rule_engine.hpp"


constexpr int window_width = 800;
constexpr int window_height = 1000;

void test_pinned_piece() {
    chess_board board;
    rule_engine rules;

    board.clear();

    // 黑将
    board.at({ 0, 4 }) = piece{
        piece_type::General,
        piece_side::Black
    };

    // 挡住红车的黑车
    board.at({ 5, 4 }) = piece{
        piece_type::Ju,
        piece_side::Black
    };

    // 红车
    board.at({ 9, 4 }) = piece{
        piece_type::Ju,
        piece_side::Red
    };

    // 红帅放在其他列，避免将帅照面
    board.at({ 9, 3 }) = piece{
        piece_type::General,
        piece_side::Red
    };

    a_move expose_general{
        {5, 4},
        {6, 4}
    };
    // 但会暴露黑将，因此完整规则拒绝
    assert(
        rules.is_legal_move(
            board,
            expose_general
        )
    );
}
int main(){
    test_pinned_piece();
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