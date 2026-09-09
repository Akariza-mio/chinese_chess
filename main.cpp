#include <cassert>
#include "raylib.h"
#include "board_painter.hpp"
#include "chess_board.hpp"
#include "rule_engine.hpp"


constexpr int window_width = 800;
constexpr int window_height = 1000;

bool find_move(const std::vector<a_move>& legal_move, const a_move& m) {
    for (const a_move& tp : legal_move) {
        if (tp == m) {
            return true;
        }
    }
    return false;
}
void test_generate_legal_moves() {
    chess_board board;
    rule_engine rules;

    board.clear();

    board.at({ 0, 4 }) = piece{
        piece_type::General,
        piece_side::Black
    };

    board.at({ 9, 3 }) = piece{
        piece_type::General,
        piece_side::Red
    };

    board.at({ 4, 4 }) = piece{
        piece_type::Ju,
        piece_side::Black
    };

    std::vector<a_move> moves =
        rules.generate_legal_moves(
            board,
            piece_side::Black
        );
    a_move m{ {0,4},{0,3} };
    assert(find_move(moves, m));
}
int main(){
    test_generate_legal_moves();
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