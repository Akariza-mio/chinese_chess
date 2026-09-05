#pragma once
#include "chess_board.hpp"
class rule_engine {
public:
	//当前只用于测试基本的行走是否合法，而不考虑是否影响将帅被杀，后续需要拓展
	bool is_basic_move_valid(const chess_board& board, const a_move& m)const;
	
private:
	int pieces_cnt_between(const chess_board& board, const pos& from,const pos& to)const;
	bool is_ju_move_ok(const chess_board& board, const a_move& m)const;
	bool is_pao_move_ok(const chess_board& board, const a_move& m)const;
	bool is_ma_move_ok(const chess_board& board, const a_move& m)const;
	bool is_xiang_move_ok(const chess_board& board, const a_move& m)const;

};