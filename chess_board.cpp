#include "chess_board.hpp"

chess_board::chess_board() {
	reset();
}
bool chess_board::is_inside(const pos& p) const {
	return p.col < 9 && p.col >= 0 && p.row >= 0 && p.row < 10;
}
std::optional<piece>& chess_board::at(const pos& p) {
	return board[p.row][p.col];
}
const std::optional<piece>& chess_board::at(const pos& p) const {
	return board[p.row][p.col];
}
void chess_board::clear() {
	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 9; ++j) {
			board[i][j].reset();
		}
	}
}
void chess_board::reset() {
	clear();
	for (int j = 0; j < 9; ++j) {
		if (j == 0 || j == 8) {
			board[0][j] = piece(piece_type::Ju,piece_side::Black);
			board[9][j] = piece(piece_type::Ju, piece_side::Red);
			board[3][j] = piece(piece_type::Bing, piece_side::Black);
			board[6][j] = piece(piece_type::Bing, piece_side::Red);
		}
		else if (j == 1 || j == 7) {
			board[0][j] = piece(piece_type::Ma, piece_side::Black);
			board[9][j] = piece(piece_type::Ma, piece_side::Red);
			board[2][j] = piece(piece_type::Pao, piece_side::Black);
			board[7][j] = piece(piece_type::Pao, piece_side::Red);
		}
		else if (j == 2 || j == 6) {
			board[0][j] = piece(piece_type::Xiang, piece_side::Black);
			board[9][j] = piece(piece_type::Xiang, piece_side::Red);
			board[3][j] = piece(piece_type::Bing, piece_side::Black);
			board[6][j] = piece(piece_type::Bing, piece_side::Red);
		}
		else if (j == 3 || j == 5) {
			board[0][j] = piece(piece_type::Shi, piece_side::Black);
			board[9][j] = piece(piece_type::Shi, piece_side::Red);
		}
		else {
			board[0][j] = piece(piece_type::General, piece_side::Black);
			board[9][j] = piece(piece_type::General, piece_side::Red);
			board[3][j] = piece(piece_type::Bing, piece_side::Black);
			board[6][j] = piece(piece_type::Bing, piece_side::Red);
		}
	}
}
void chess_board::apply_move(const a_move& m) {
	std::optional<piece> from_cell = at(m.from);
	at(m.to) = from_cell;
	at(m.from).reset();
}
