#include <optional>
#include "raylib.h"
#include "board_painter.hpp"
#include "chess_board.hpp"
#include "rule_engine.hpp"
constexpr int window_width = 800;
constexpr int window_height = 1000;

void handle_click(chess_board& board, std::optional<pos>& selected_pos, piece_side& now_turn,
	board_painter* bp,rule_engine* rule) {
	if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		return;
	}
	std::optional<pos> click_pos = bp->get_pos_from_mouse(GetMouseX(), GetMouseY());
	if (!click_pos.has_value()) {
		selected_pos.reset();
		return;
	}
	const std::optional<piece>& click_cell = board.at(*click_pos);
	if (!selected_pos.has_value()) {
		if (click_cell.has_value() && click_cell->get_side() == now_turn) {
			selected_pos = click_pos;
		}
	}
	else {
		if (click_cell.has_value() && click_cell->get_side() == now_turn) {
			selected_pos = click_pos;
		}
		else {
			a_move m{ *selected_pos,*click_pos };
			if (rule->is_legal_move(board, m)) {
				board.apply_move(m);
				now_turn = (now_turn == piece_side::Red) ? piece_side::Black : piece_side::Red;
			}
			selected_pos.reset();
		}
	}
}
int main(){
    chess_board board;
    InitWindow(window_width, window_height, "Chinese Chess");
    SetTargetFPS(60);
    board_painter* bp = new board_painter();
    rule_engine* rule = new rule_engine();
    std::optional<pos> selected_pos = std::nullopt;
    piece_side now_turn = piece_side::Red;
	game_status now_status = rule->get_status(board, now_turn);
	bool is_checking = rule->is_in_check(board, now_turn);
	while (!WindowShouldClose()) {
		now_status = rule->get_status(board, now_turn);
		is_checking = rule->is_in_check(board, now_turn);
		if (now_status == game_status::goingOn) {
			handle_click(board, selected_pos, now_turn, bp, rule);
		}
		else {
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				Vector2 mouse_point = { GetMouseX(), GetMouseY() };
				Rectangle restart_area = { 300, 520, 200, 80 };
				if (CheckCollisionPointRec(mouse_point, restart_area)) {
					board.reset();
					now_turn = piece_side::Red;
					selected_pos.reset();
				}
			}
		}
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
		bp->draw_status(now_turn, is_checking);
		if (now_status != game_status::goingOn) {
			bp->draw_game_over_window(now_status);
		}
        EndDrawing();
    }
    delete bp;
    delete rule;
    CloseWindow();
}
