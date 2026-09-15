#include <optional>
#include "raylib.h"
#include "board_painter.hpp"
#include "game_state.hpp"
#include "rule_engine.hpp"
constexpr int window_width = 800;
constexpr int window_height = 1000;

void handle_click(game_state& game, std::optional<pos>& selected_pos,
	board_painter* bp,rule_engine* rule) {
	if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		return;
	}
	std::optional<pos> click_pos = bp->get_pos_from_mouse(GetMouseX(), GetMouseY());
	if (!click_pos.has_value()) {
		selected_pos.reset();
		return;
	}
	const chess_board& board = game.board();
	const std::optional<piece>& click_cell = board.at(*click_pos);
	if (!selected_pos.has_value()) {
		if (click_cell.has_value() && click_cell->get_side() == game.current_turn()) {
			selected_pos = click_pos;
		}
	}
	else {
		if (click_cell.has_value() && click_cell->get_side() == game.current_turn()) {
			selected_pos = click_pos;
		}
		else {
			a_move m{ *selected_pos,*click_pos };
			game.try_move(m, *rule);
			selected_pos.reset();
		}
	}
}
int main(){
    game_state game;
    InitWindow(window_width, window_height, "Chinese Chess");
    SetTargetFPS(60);
    board_painter* bp = new board_painter();
    rule_engine* rule = new rule_engine();
    std::optional<pos> selected_pos = std::nullopt;
	while (!WindowShouldClose()) {
		if (game.status() == game_status::goingOn) {
			handle_click(game, selected_pos, bp, rule);
		}
		else {
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				Vector2 mouse_point = {
					static_cast<float>(GetMouseX()),
					static_cast<float>(GetMouseY())
				};
				Rectangle restart_area = { 300, 520, 200, 80 };
				if (CheckCollisionPointRec(mouse_point, restart_area)) {
					game.reset();
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
		bp->draw_pieces(game.board());
        if (selected_pos.has_value()) {
            bp->draw_selected_sign(*selected_pos);
        }
		const bool is_checking = rule->is_in_check(
			game.board(), game.current_turn()
		);
		bp->draw_status(game.current_turn(), is_checking);
		if (game.status() != game_status::goingOn) {
			bp->draw_game_over_window(game.status());
		}
        EndDrawing();
    }
    delete bp;
    delete rule;
    CloseWindow();
}
