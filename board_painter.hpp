#pragma once
#include "raylib.h"

constexpr int grid_wid = 80;
constexpr int original_x = 80;
constexpr int original_y = 180;
constexpr int chess_width = 640;
constexpr int chess_height = 720;
class board_painter {
public:
    board_painter();
	~board_painter();
	void draw_jin(int x,int y,int side) const;
	void draw_board() const;

private:
	Font chinese_font{};
};