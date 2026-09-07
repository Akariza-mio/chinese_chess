#include "rule_engine.hpp"

bool rule_engine::is_basic_move_valid(const chess_board& board, const a_move& m)const {
	//终点和起点都需要在棋盘上
	if (!board.is_inside(m.from) || !board.is_inside(m.to)) {
		return false;
	}
	//终点和起点不能相同
	if (m.from == m.to) {
		return false;
	}
	//起点需要有棋子
	const std::optional<piece>& from_cell = board.at(m.from);
	if (!from_cell.has_value()) {
		return false;
	}
	//终点不能是己方的棋子
	const std::optional<piece>& to_cell = board.at(m.to);
	if (to_cell.has_value() && to_cell->get_side() == from_cell->get_side()) {
		return false;
	}
	//根据棋子类型判断走法
	switch (from_cell->get_type()) {
	case piece_type::Ju:
		return is_ju_move_ok(board, m);
	case piece_type::Pao:
		return is_pao_move_ok(board, m);
	case piece_type::Ma:
		return is_ma_move_ok(board, m);
	case piece_type::Xiang:
		return is_xiang_move_ok(board, m);
	case piece_type::Shi:
		return is_shi_move_ok(board, m);
	case piece_type::General:
		return is_general_move_ok(board, m);
	case piece_type::Bing:
		return is_bing_move_ok(board, m);
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
bool rule_engine::is_ma_move_ok(const chess_board& board, const a_move& m)const {
	int row_dif = m.to.row - m.from.row;
	int col_dif = m.to.col - m.from.col;
	bool vertical = (std::abs(row_dif) == 2 && std::abs(col_dif) == 1);
	bool horizontal = (std::abs(row_dif) == 1 && std::abs(col_dif) == 2);
	if (!vertical && !horizontal) {
		return false;
	}
	pos leg;
	if (std::abs(row_dif) == 2) {
		leg = { m.from.row + row_dif / 2,m.from.col };
	}
	else {
		leg = { m.from.row ,m.from.col + col_dif / 2 };
	}
	if (board.at(leg).has_value()) {
		return false;
	}
	return true;
}
bool rule_engine::is_xiang_move_ok(const chess_board& board, const a_move& m)const {
	piece_side side = board.at(m.from)->get_side();
	if (side == piece_side::Black && m.to.row > 4) {
		return false;
	}
	if (side == piece_side::Red && m.to.row < 5) {
		return false;
	}
	int row_dif = m.to.row - m.from.row;
	int col_dif = m.to.col - m.from.col;
	if (std::abs(row_dif) != 2 || std::abs(col_dif) != 2) {
		return false;
	}
	pos leg{ m.from.row + row_dif / 2,m.from.col + col_dif / 2 };
	if (board.at(leg).has_value()) {
		return false;
	}
	return true;
}
bool rule_engine::is_shi_move_ok(const chess_board& board, const a_move& m)const {
	if (m.to.col < 3 || m.to.col > 5) {
		return false;
	}
	piece_side side = board.at(m.from)->get_side();
	if (side == piece_side::Black && m.to.row > 2) {
		return false;
	}
	if (side == piece_side::Red && m.to.row < 7) {
		return false;
	}
	int row_dif = m.to.row - m.from.row;
	int col_dif = m.to.col - m.from.col;
	if (std::abs(row_dif) != 1 || std::abs(col_dif) != 1) {
		return false;
	}
	return true;
}
bool rule_engine::is_general_move_ok(const chess_board& board, const a_move& m)const {
	if (m.to.col < 3 || m.to.col > 5) {
		return false;
	}
	piece_side side = board.at(m.from)->get_side();
	if (side == piece_side::Black && m.to.row > 2) {
		return false;
	}
	if (side == piece_side::Red && m.to.row < 7) {
		return false;
	}
	int row_dif = m.to.row - m.from.row;
	int col_dif = m.to.col - m.from.col;
	if (std::abs(row_dif) > 1 || std::abs(col_dif) > 1) {
		return false;
	}
	if (std::abs(row_dif) == 1 && std::abs(col_dif) == 1) {
		return false;
	}
	return true;
}
bool rule_engine::is_bing_move_ok(const chess_board& board, const a_move& m)const {
	int row_dif = m.to.row - m.from.row;
	int col_dif = m.to.col - m.from.col;
	if (std::abs(row_dif) + std::abs(col_dif) != 1) {
		return false;
	}
	piece_side side = board.at(m.from)->get_side();
	if (side == piece_side::Black) {
		if (row_dif == -1) {
			return false;
		}
		if (m.from.row < 5 && std::abs(col_dif) == 1) {
			return false;
		}
	}
	else {
		if (row_dif == 1) {
			return false;
		}
		if (m.from.row > 4 && std::abs(col_dif) == 1) {
			return false;
		}
	}
	return true;
}