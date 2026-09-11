#include <cmath>
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
pos rule_engine::find_general(const chess_board& board, piece_side side)const {
	for (int row = 0; row < 10; ++row) {
		for (int col = 0; col < 9; ++col) {
			pos now{ row,col };
			const std::optional<piece>& cell = board.at(now);
			if (cell.has_value()) {
				if (cell->get_side() == side && cell->get_type() == piece_type::General) {
					return now;
				}
			}
		}
	}
	return { -1,-1 };
}
bool rule_engine::is_general_facing(const chess_board& board)const {
	pos jiang = find_general(board, piece_side::Black);
	pos shuai = find_general(board, piece_side::Red);
	if (!board.is_inside(jiang) || !board.is_inside(shuai)) {
		return false;
	}
	if (jiang.col != shuai.col) {
		return false;
	}
	if (pieces_cnt_between(board, jiang, shuai) != 0) {
		return false;
	}
	return true;
}
bool rule_engine::is_in_check(const chess_board& board, piece_side side)const {
	pos general = find_general(board, side);
	if (!board.is_inside(general)) {
		return false;
	}
	if (is_general_facing(board)) {
		return true;
	}
	for (int row = 0; row < 10; ++row) {
		for (int col = 0; col < 9; ++col) {
			pos attacker{ row,col };
			if (!board.at(attacker).has_value()) {
				continue;
			}
			a_move m{ attacker,general };
			if (is_basic_move_valid(board, m)) {
				return true;
			}
		}
	}
	return false;
}
bool rule_engine::is_legal_move(const chess_board& board, const a_move& m)const {
	if (!is_basic_move_valid(board, m)) {
		return false;
	}
	chess_board tp_board = board;
	piece_side side = board.at(m.from)->get_side();
	std::optional<piece> from_piece = board.at(m.from);
	tp_board.at(m.to) = from_piece;
	tp_board.at(m.from).reset();
	return !is_in_check(tp_board, side);
}
std::vector<a_move> rule_engine::generate_legal_moves(const chess_board& board, piece_side side)const {
	std::vector<a_move> legal_moves;
	legal_moves.reserve(200);
	for (int from_row = 0; from_row < 10; ++from_row) {
		for (int from_col = 0; from_col < 9; ++from_col) {
			pos from{ from_row,from_col };
			const std::optional<piece>& from_cell = board.at(from);
			if (!from_cell.has_value() || from_cell->get_side() != side) {
				continue;
			}
			for (int to_row = 0; to_row < 10; ++to_row) {
				for (int to_col = 0; to_col < 9; ++to_col) {
					pos to{ to_row,to_col };
					a_move m{ from,to };
					if (is_legal_move(board, m)) {
						legal_moves.push_back(m);
					}
				}
			}
		}
	}
	return legal_moves;
}
game_status rule_engine::get_status(const chess_board& board, piece_side side)const {
	std::vector<a_move> legal_moves = generate_legal_moves(board,side);
	if (!legal_moves.empty()) {
		return game_status::goingOn;
	}
	bool checking = is_in_check(board, side);
	if (side == piece_side::Red) {
		return checking ? game_status::BlackWin_checkmate : game_status::BlackWin_stalemate;
	}
	return checking ? game_status::RedWin_checkmate : game_status::RedWin_stalemate;
}
std::optional<pos> rule_engine::get_pos_from_mouse(int mouse_x, int mouse_y)const {
	float col_mouse = (mouse_x - original_x) / (float)grid_wid;
	float row_mouse = (mouse_y - original_y) / (float)grid_wid;
	int col = std::round(col_mouse);
	int row = std::round(row_mouse);
	if (row > 9 || row < 0 || col > 8 || col < 0) {
		return std::nullopt;
	}
	int center_x = original_x + col * grid_wid;
	int center_y = original_y + row * grid_wid;
	int dx = mouse_x - center_x;
	int dy = mouse_y - center_y;
	if (dx * dx + dy * dy <= 35 * 35) {
		return pos{ row,col };
	}
	return std::nullopt;
}