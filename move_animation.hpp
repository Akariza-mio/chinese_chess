#pragma once

#include <optional>

#include "chess_board.hpp"

class move_animation {
public:
    void start(const chess_board& board_before_move, const a_move& move);
    void update(float delta_seconds);
    void reset();

    bool active() const;
    float progress() const;
    const a_move& move() const;
    const piece& moving_piece() const;
    const std::optional<piece>& captured_piece() const;

private:
    static constexpr float duration_seconds_ = 0.20f;

    bool active_ = false;
    float elapsed_seconds_ = 0.0f;
    a_move move_{};
    std::optional<piece> moving_piece_;
    std::optional<piece> captured_piece_;
};
