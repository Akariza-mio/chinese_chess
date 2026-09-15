#include "game_state.hpp"

const chess_board& game_state::board() const {
    return board_;
}

piece_side game_state::current_turn() const {
    return current_turn_;
}

game_status game_state::status() const {
    return status_;
}

std::uint32_t game_state::sequence() const {
    return sequence_;
}

bool game_state::try_move(const a_move& move, const rule_engine& rules) {
    if (status_ != game_status::goingOn) {
        return false;
    }
    if (!board_.is_inside(move.from) || !board_.is_inside(move.to)) {
        return false;
    }

    const std::optional<piece>& from_cell = board_.at(move.from);
    if (!from_cell.has_value() || from_cell->get_side() != current_turn_) {
        return false;
    }
    if (!rules.is_legal_move(board_, move)) {
        return false;
    }

    board_.apply_move(move);
    current_turn_ = (current_turn_ == piece_side::Red)
        ? piece_side::Black
        : piece_side::Red;
    ++sequence_;
    status_ = rules.get_status(board_, current_turn_);
    return true;
}

void game_state::reset() {
    board_.reset();
    current_turn_ = piece_side::Red;
    status_ = game_status::goingOn;
    sequence_ = 0;
}

void game_state::apply_snapshot(
    const chess_board& board,
    piece_side current_turn,
    game_status status,
    std::uint32_t sequence
) {
    board_ = board;
    current_turn_ = current_turn;
    status_ = status;
    sequence_ = sequence;
}
