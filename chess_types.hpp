#pragma once
enum class piece_side {
	Black,
	Red
};
enum class piece_type {
	General,
	Shi,
	Xiang,
	Ma,
	Ju,
	Pao,
	Bing
};
//位置,row行，col列
struct pos {
	int row;
	int col;
	bool operator==(const pos& other) const {
		return row == other.row && col == other.col;
	}
};
struct a_move {
	pos from;
	pos to;
	bool operator==(const a_move& m) const = default;
};
class piece {
public:
	piece(piece_type type, piece_side side) : type(type), side(side) {};
	piece_type get_type() const{
		return type;
	}
	piece_side get_side() const{
		return side;
	}
private:
	piece_type type;
	piece_side side;
};
