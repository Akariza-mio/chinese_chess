#pragma once
#include "raylib.h"
#include "chess_board.hpp"
#include <string>
#include <vector>

class move_animation;

constexpr int grid_wid = 80;
constexpr int original_x = 80;
constexpr int original_y = 180;
constexpr int chess_width = 640;
constexpr int chess_height = 720;

constexpr pos orient_board_position(const pos& logical, bool flipped) {
	return flipped
		? pos{ 9 - logical.row, 8 - logical.col }
		: logical;
}

class board_painter {
public:
    board_painter();
	~board_painter();
	void draw_jin(int x,int y,int side) const;
	void draw_board(bool flipped = false) const;
	void draw_pieces(
		const chess_board& board,
		bool flipped = false,
		std::optional<pos> hidden_position = std::nullopt
	) const;
	void draw_moving_piece(
		const move_animation& animation,
		bool flipped = false
	) const;
	void draw_selected_sign(const pos& p, bool flipped = false)const;
	std::optional<pos> get_pos_from_mouse(
		int mouse_x,
		int mouse_y,
		bool flipped = false
	)const;
	const char* get_piece_text(piece_type type, piece_side side)const;
	void draw_status(piece_side now_turn, bool is_checking)const;
	void draw_game_over_window(
		game_status status,
		const char* button_text = "重开"
	) const;
	void draw_main_menu() const;
	void draw_join_screen(const std::string& ip, const std::string& message) const;
	void draw_connection_screen(
		const std::string& message,
		const std::vector<std::string>& local_addresses
	) const;
	void draw_message_screen(
		const char* title,
		const std::string& message
	) const;
	void draw_back_button() const;
private:
	void draw_piece_at(const piece& value, float center_x, float center_y) const;
	Font chinese_font{};
};
