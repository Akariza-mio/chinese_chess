#include "rule_engine.hpp"

bool rule_engine::is_basic_move_valid(const chess_board& board, const a_move& m)const {
	if (!board.is_inside(m.from) || !board.is_inside(m.to)) {
		return false;
	}
	if (m.from == m.to) {
		return false;
	}
	const std::optional<piece>& from_cell = board.at(m.from);
	if (!from_cell.has_value()) {
		return false;
	}
	const std::optional<piece>& to_cell = board.at(m.to);
	if (to_cell.has_value() && to_cell->get_side() == from_cell->get_side()) {
		return false;
	}
	switch (from_cell->get_type()) {
	case piece_type::Ju:
		return is_ju_move_ok(board, m);
	case piece_type::Pao:
		return is_pao_move_ok(board, m);
	default:
		return false;
	}
}

int rule_engine::pieces_cnt_between(const chess_board& board, const pos& from, const pos& to)const {
	int res = -1;
	if (from.row != to.row && from.col != to.col) {
		return -1;
	}
	if (board.is_inside(from) && board.is_inside(to)) {
		if (from == to) {
			return 0;
		}
		res = 0;
		int row_d = 0;
		int col_d = 0;
		if (to.row > from.row) {
			row_d = 1;
		}
		else if (to.row < from.row) {
			row_d = -1;
		}
		if (to.col > from.col) {
			col_d = 1;
		}
		else if (to.col < from.col) {
			col_d = -1;
		}
		pos now_pos{ from.row + row_d,from.col + col_d };
		while (now_pos != to) {
			if (board.at(now_pos).has_value()) {
				++res;
			}
			now_pos.row += row_d;
			now_pos.col += col_d;
		}
		return res;
	}
	return res;
}
bool rule_engine::is_ju_move_ok(const chess_board& board, const a_move& m)const {
	bool row_eq = m.from.row == m.to.row;
	bool col_eq = m.from.col == m.to.col;
	//车的行走只需要为直线且中途没有棋子即可
	if ((row_eq || col_eq) && pieces_cnt_between(board, m.from, m.to) == 0) {
		return true;
	}
	return false;
}
bool rule_engine::is_pao_move_ok(const chess_board& board, const a_move& m)const{
	bool row_eq = m.from.row == m.to.row;
	bool col_eq = m.from.col == m.to.col;
	//炮的终点如果有棋子，则中间必须隔一个
	if (board.at(m.to).has_value()) {
		if ((row_eq || col_eq) && pieces_cnt_between(board, m.from, m.to) == 1) {
			return true;
		}
		return false;
	}
	//如果终点没有棋子，则需要中间无棋子
	if ((row_eq || col_eq) && pieces_cnt_between(board, m.from, m.to) == 0) {
		return true;
	}
	return false;
}