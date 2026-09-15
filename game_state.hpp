#pragma once

#include <cstdint>
#include <optional>

#include "chess_board.hpp"
#include "rule_engine.hpp"

class game_state {
public:
    game_state() = default;

    const chess_board& board() const;
    piece_side current_turn() const;
    game_status status() const;
    std::uint32_t sequence() const;
    const std::optional<a_move>& last_move() const;

    bool try_move(const a_move& move, const rule_engine& rules);
    void reset();

    // 客户端收到主机的完整状态后使用。状态的合法性由协议解析层检查。
    void apply_snapshot(
        const chess_board& board,
        piece_side current_turn,
        game_status status,
        std::uint32_t sequence,
        std::optional<a_move> last_move
    );

private:
    chess_board board_{};
    piece_side current_turn_ = piece_side::Red;
    game_status status_ = game_status::goingOn;
    std::uint32_t sequence_ = 0;
    std::optional<a_move> last_move_;
};
