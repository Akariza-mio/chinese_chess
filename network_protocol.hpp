#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

#include "chess_board.hpp"

constexpr std::uint32_t network_protocol_version = 3;

struct hello_message {
    std::uint32_t version;
};

struct role_message {
    piece_side side;
};

struct move_message {
    std::uint32_t sequence;
    a_move move;
};

struct state_message {
    std::uint32_t sequence;
    piece_side current_turn;
    game_status status;
    std::optional<a_move> last_move;
    bool red_can_undo;
    bool black_can_undo;
    chess_board board;
};

struct error_message {
    std::string reason;
};

struct restart_message {};
struct undo_request_message {};

struct undo_response_message {
    bool accepted;
};

struct quit_message {};

using network_message = std::variant<
    hello_message,
    role_message,
    move_message,
    state_message,
    error_message,
    restart_message,
    undo_request_message,
    undo_response_message,
    quit_message
>;

std::string serialize_message(const network_message& message);

// line 不包含末尾换行符。失败时返回 nullopt，并在 error 中写入原因。
std::optional<network_message> parse_message(
    std::string_view line,
    std::string& error
);
