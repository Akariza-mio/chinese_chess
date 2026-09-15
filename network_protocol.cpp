#include "network_protocol.hpp"

#include <charconv>
#include <sstream>
#include <type_traits>
#include <utility>

namespace {

std::string side_to_text(piece_side side) {
    return side == piece_side::Red ? "RED" : "BLACK";
}

std::optional<piece_side> text_to_side(std::string_view text) {
    if (text == "RED") return piece_side::Red;
    if (text == "BLACK") return piece_side::Black;
    return std::nullopt;
}

std::string status_to_text(game_status status) {
    switch (status) {
    case game_status::goingOn:             return "PLAYING";
    case game_status::RedWin_stalemate:    return "RED_STALEMATE";
    case game_status::RedWin_checkmate:    return "RED_CHECKMATE";
    case game_status::BlackWin_stalemate:  return "BLACK_STALEMATE";
    case game_status::BlackWin_checkmate:  return "BLACK_CHECKMATE";
    case game_status::Disconnected:        return "DISCONNECTED";
    }
    return "DISCONNECTED";
}

std::optional<game_status> text_to_status(std::string_view text) {
    if (text == "PLAYING") return game_status::goingOn;
    if (text == "RED_STALEMATE") return game_status::RedWin_stalemate;
    if (text == "RED_CHECKMATE") return game_status::RedWin_checkmate;
    if (text == "BLACK_STALEMATE") return game_status::BlackWin_stalemate;
    if (text == "BLACK_CHECKMATE") return game_status::BlackWin_checkmate;
    if (text == "DISCONNECTED") return game_status::Disconnected;
    return std::nullopt;
}

char encode_piece(const piece& value) {
    char code = '?';
    switch (value.get_type()) {
    case piece_type::General: code = 'K'; break;
    case piece_type::Shi:     code = 'A'; break;
    case piece_type::Xiang:   code = 'E'; break;
    case piece_type::Ma:      code = 'H'; break;
    case piece_type::Ju:      code = 'R'; break;
    case piece_type::Pao:     code = 'C'; break;
    case piece_type::Bing:    code = 'P'; break;
    }

    if (value.get_side() == piece_side::Red) {
        code = static_cast<char>(code - 'A' + 'a');
    }
    return code;
}

std::optional<piece> decode_piece(char code) {
    piece_side side = piece_side::Black;
    if (code >= 'a' && code <= 'z') {
        side = piece_side::Red;
        code = static_cast<char>(code - 'a' + 'A');
    }

    switch (code) {
    case 'K': return piece(piece_type::General, side);
    case 'A': return piece(piece_type::Shi, side);
    case 'E': return piece(piece_type::Xiang, side);
    case 'H': return piece(piece_type::Ma, side);
    case 'R': return piece(piece_type::Ju, side);
    case 'C': return piece(piece_type::Pao, side);
    case 'P': return piece(piece_type::Bing, side);
    default:  return std::nullopt;
    }
}

std::string encode_board(const chess_board& board) {
    std::string result;
    result.reserve(90);
    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < 9; ++col) {
            const std::optional<piece>& cell = board.at({ row, col });
            result.push_back(cell.has_value() ? encode_piece(*cell) : '.');
        }
    }
    return result;
}

std::optional<chess_board> decode_board(
    std::string_view text,
    std::string& error
) {
    if (text.size() != 90) {
        error = "STATE board must contain exactly 90 cells";
        return std::nullopt;
    }

    chess_board board;
    board.clear();
    for (std::size_t index = 0; index < text.size(); ++index) {
        if (text[index] == '.') continue;

        std::optional<piece> value = decode_piece(text[index]);
        if (!value.has_value()) {
            error = "STATE board contains an unknown piece code";
            return std::nullopt;
        }
        const int row = static_cast<int>(index / 9);
        const int col = static_cast<int>(index % 9);
        board.at({ row, col }) = *value;
    }
    return board;
}

bool parse_uint32(std::string_view text, std::uint32_t& value) {
    if (text.empty()) return false;
    const char* begin = text.data();
    const char* end = begin + text.size();
    const auto result = std::from_chars(begin, end, value);
    return result.ec == std::errc{} && result.ptr == end;
}

bool parse_coordinate(std::string_view text, int maximum, int& value) {
    if (text.empty()) return false;
    const char* begin = text.data();
    const char* end = begin + text.size();
    const auto result = std::from_chars(begin, end, value);
    return result.ec == std::errc{} && result.ptr == end &&
        value >= 0 && value <= maximum;
}

bool has_extra_token(std::istringstream& stream) {
    std::string extra;
    return static_cast<bool>(stream >> extra);
}

} // namespace

std::string serialize_message(const network_message& message) {
    return std::visit([](const auto& value) -> std::string {
        using message_type = std::decay_t<decltype(value)>;

        if constexpr (std::is_same_v<message_type, hello_message>) {
            return "HELLO " + std::to_string(value.version) + "\n";
        }
        else if constexpr (std::is_same_v<message_type, role_message>) {
            return "ROLE " + side_to_text(value.side) + "\n";
        }
        else if constexpr (std::is_same_v<message_type, move_message>) {
            return "MOVE " + std::to_string(value.sequence) + " " +
                std::to_string(value.move.from.row) + " " +
                std::to_string(value.move.from.col) + " " +
                std::to_string(value.move.to.row) + " " +
                std::to_string(value.move.to.col) + "\n";
        }
        else if constexpr (std::is_same_v<message_type, state_message>) {
            return "STATE " + std::to_string(value.sequence) + " " +
                side_to_text(value.current_turn) + " " +
                status_to_text(value.status) + " " +
                encode_board(value.board) + "\n";
        }
        else if constexpr (std::is_same_v<message_type, error_message>) {
            return "ERROR " + value.reason + "\n";
        }
        else {
            return "QUIT\n";
        }
    }, message);
}

std::optional<network_message> parse_message(
    std::string_view line,
    std::string& error
) {
    error.clear();
    if (!line.empty() && line.back() == '\r') {
        line.remove_suffix(1);
    }
    if (line.empty()) {
        error = "empty message";
        return std::nullopt;
    }

    std::istringstream stream{ std::string(line) };
    std::string command;
    stream >> command;

    if (command == "HELLO") {
        std::string version_text;
        std::uint32_t version = 0;
        if (!(stream >> version_text) ||
            !parse_uint32(version_text, version) ||
            has_extra_token(stream)) {
            error = "invalid HELLO message";
            return std::nullopt;
        }
        return hello_message{ version };
    }

    if (command == "ROLE") {
        std::string side_text;
        if (!(stream >> side_text) || has_extra_token(stream)) {
            error = "invalid ROLE message";
            return std::nullopt;
        }
        const std::optional<piece_side> side = text_to_side(side_text);
        if (!side.has_value()) {
            error = "ROLE contains an unknown side";
            return std::nullopt;
        }
        return role_message{ *side };
    }

    if (command == "MOVE") {
        std::string sequence_text;
        std::string from_row_text;
        std::string from_col_text;
        std::string to_row_text;
        std::string to_col_text;
        if (!(stream >> sequence_text >> from_row_text >> from_col_text >>
            to_row_text >> to_col_text) || has_extra_token(stream)) {
            error = "invalid MOVE message";
            return std::nullopt;
        }

        std::uint32_t sequence = 0;
        a_move move{};
        if (!parse_uint32(sequence_text, sequence) ||
            !parse_coordinate(from_row_text, 9, move.from.row) ||
            !parse_coordinate(from_col_text, 8, move.from.col) ||
            !parse_coordinate(to_row_text, 9, move.to.row) ||
            !parse_coordinate(to_col_text, 8, move.to.col)) {
            error = "MOVE contains an invalid number or coordinate";
            return std::nullopt;
        }
        return move_message{ sequence, move };
    }

    if (command == "STATE") {
        std::string sequence_text;
        std::string side_text;
        std::string status_text;
        std::string board_text;
        if (!(stream >> sequence_text >> side_text >> status_text >> board_text) ||
            has_extra_token(stream)) {
            error = "invalid STATE message";
            return std::nullopt;
        }

        std::uint32_t sequence = 0;
        const std::optional<piece_side> side = text_to_side(side_text);
        const std::optional<game_status> status = text_to_status(status_text);
        const std::optional<chess_board> board = decode_board(board_text, error);
        if (!parse_uint32(sequence_text, sequence) ||
            !side.has_value() || !status.has_value() || !board.has_value()) {
            if (error.empty()) error = "STATE contains an invalid field";
            return std::nullopt;
        }
        return state_message{ sequence, *side, *status, *board };
    }

    if (command == "ERROR") {
        std::string reason;
        std::getline(stream >> std::ws, reason);
        if (reason.empty()) {
            error = "ERROR message has no reason";
            return std::nullopt;
        }
        return error_message{ std::move(reason) };
    }

    if (command == "QUIT") {
        if (has_extra_token(stream)) {
            error = "invalid QUIT message";
            return std::nullopt;
        }
        return quit_message{};
    }

    error = "unknown message type";
    return std::nullopt;
}
