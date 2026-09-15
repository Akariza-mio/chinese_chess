#include "move_animation.hpp"

#include <algorithm>
#include <cassert>

void move_animation::start(
    const chess_board& board_before_move,
    const a_move& move
) {
    if (!board_before_move.is_inside(move.from) ||
        !board_before_move.is_inside(move.to) ||
        !board_before_move.at(move.from).has_value()) {
        reset();
        return;
    }

    move_ = move;
    moving_piece_ = board_before_move.at(move.from);
    captured_piece_ = board_before_move.at(move.to);
    elapsed_seconds_ = 0.0f;
    active_ = true;
}

void move_animation::update(float delta_seconds) {
    if (!active_) return;

    elapsed_seconds_ += std::max(delta_seconds, 0.0f);
    if (elapsed_seconds_ >= duration_seconds_) {
        elapsed_seconds_ = duration_seconds_;
        active_ = false;
    }
}

void move_animation::reset() {
    active_ = false;
    elapsed_seconds_ = 0.0f;
    moving_piece_.reset();
    captured_piece_.reset();
}

bool move_animation::active() const {
    return active_;
}

float move_animation::progress() const {
    const float linear = std::clamp(
        elapsed_seconds_ / duration_seconds_, 0.0f, 1.0f
    );
    return linear * linear * (3.0f - 2.0f * linear);
}

const a_move& move_animation::move() const {
    assert(moving_piece_.has_value());
    return move_;
}

const piece& move_animation::moving_piece() const {
    assert(moving_piece_.has_value());
    return *moving_piece_;
}

const std::optional<piece>& move_animation::captured_piece() const {
    return captured_piece_;
}
