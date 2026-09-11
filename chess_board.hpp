#pragma once
#include <array>
#include <optional>
#include "chess_types.hpp"

class chess_board {
public:
	chess_board();
	bool is_inside(const pos& p) const;
	std::optional<piece>& at(const pos& p);
	const std::optional<piece>& at(const pos& p) const;
	void apply_move(const a_move& m);
	void clear();
	void reset();
private:
	std::array<std::array<std::optional<piece>, 9>, 10> board{};
};