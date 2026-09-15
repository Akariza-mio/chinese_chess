#include "network_game_session.hpp"

#include <utility>

#include "network_protocol.hpp"

bool network_game_session::host(std::string& error, std::uint16_t port) {
    stop();
    game_.reset();
    role_ = role::host_red;
    state_ = network_session_state::hosting;
    status_text_ = "Waiting for another player";
    peer_hello_received_ = false;
    assigned_role_received_ = false;
    initial_state_received_ = false;
    move_pending_ = false;
    restart_pending_ = false;

    if (!transport_.start_host(port, error)) {
        state_ = network_session_state::failed;
        status_text_ = error;
        role_ = role::none;
        return false;
    }
    return true;
}

bool network_game_session::join(
    std::string_view ipv4_address,
    std::string& error,
    std::uint16_t port
) {
    stop();
    game_.reset();
    role_ = role::client_black;
    state_ = network_session_state::connecting;
    status_text_ = "Connecting to host";
    peer_hello_received_ = false;
    assigned_role_received_ = false;
    initial_state_received_ = false;
    move_pending_ = false;
    restart_pending_ = false;

    if (!transport_.start_client(ipv4_address, port, error)) {
        state_ = network_session_state::failed;
        status_text_ = error;
        role_ = role::none;
        return false;
    }
    return true;
}

void network_game_session::update() {
    for (const network_event& event : transport_.take_events()) {
        handle_transport_event(event);
    }
}

bool network_game_session::submit_local_move(
    const a_move& move,
    std::string& error
) {
    error.clear();
    if (!can_local_move()) {
        error = "It is not this player's turn";
        return false;
    }

    if (role_ == role::host_red) {
        if (!game_.try_move(move, rules_)) {
            error = "Illegal move";
            return false;
        }
        if (!send_current_state()) {
            error = status_text_;
            return false;
        }
        return true;
    }

    const network_message request = move_message{
        game_.sequence(), move
    };
    if (!send_message(request)) {
        error = status_text_;
        return false;
    }
    move_pending_ = true;
    status_text_ = "Waiting for host confirmation";
    return true;
}

bool network_game_session::request_restart(std::string& error) {
    error.clear();
    if (state_ != network_session_state::playing ||
        game_.status() == game_status::goingOn) {
        error = "The game has not ended";
        return false;
    }
    if (restart_pending_) {
        error = "Restart request is already pending";
        return false;
    }

    if (role_ == role::host_red) {
        game_.reset();
        if (!send_message(restart_message{})) {
            error = status_text_;
            return false;
        }
        if (!send_current_state()) {
            error = status_text_;
            return false;
        }
        status_text_ = "Connected - you are Red";
        return true;
    }

    if (!send_message(restart_message{})) {
        error = status_text_;
        return false;
    }
    restart_pending_ = true;
    status_text_ = "Waiting for host to restart";
    return true;
}

void network_game_session::stop() {
    if (transport_.state() == network_connection_state::connected) {
        std::string ignored_error;
        transport_.send_text(
            serialize_message(quit_message{}),
            ignored_error
        );
    }
    transport_.stop();
    role_ = role::none;
    state_ = network_session_state::idle;
    status_text_ = "Not connected";
    peer_hello_received_ = false;
    assigned_role_received_ = false;
    initial_state_received_ = false;
    move_pending_ = false;
    restart_pending_ = false;
}

const game_state& network_game_session::game() const {
    return game_;
}

network_session_state network_game_session::state() const {
    return state_;
}

std::optional<piece_side> network_game_session::local_side() const {
    if (role_ == role::host_red) return piece_side::Red;
    if (role_ == role::client_black) return piece_side::Black;
    return std::nullopt;
}

bool network_game_session::can_local_move() const {
    const std::optional<piece_side> side = local_side();
    return state_ == network_session_state::playing &&
        side.has_value() &&
        game_.status() == game_status::goingOn &&
        game_.current_turn() == *side &&
        !move_pending_;
}

bool network_game_session::move_pending() const {
    return move_pending_;
}

bool network_game_session::restart_pending() const {
    return restart_pending_;
}

const std::string& network_game_session::status_text() const {
    return status_text_;
}

void network_game_session::handle_transport_event(const network_event& event) {
    switch (event.type) {
    case network_event_type::connected:
        state_ = network_session_state::handshaking;
        status_text_ = "Performing protocol handshake";
        if (!send_message(hello_message{ network_protocol_version })) return;
        break;

    case network_event_type::line_received:
        handle_line(event.text);
        break;

    case network_event_type::disconnected:
        if (state_ != network_session_state::failed) {
            state_ = network_session_state::disconnected;
            status_text_ = event.text.empty()
                ? "The other player disconnected"
                : event.text;
        }
        move_pending_ = false;
        restart_pending_ = false;
        break;

    case network_event_type::error:
        state_ = network_session_state::failed;
        status_text_ = event.text;
        move_pending_ = false;
        restart_pending_ = false;
        break;
    }
}

void network_game_session::handle_line(std::string_view line) {
    std::string error;
    const std::optional<network_message> message = parse_message(line, error);
    if (!message.has_value()) {
        fail_session("Protocol error: " + error, true);
        return;
    }

    if (role_ == role::host_red) {
        handle_host_message(*message);
    }
    else if (role_ == role::client_black) {
        handle_client_message(*message);
    }
    else {
        fail_session("Protocol message received without a session role", true);
    }
}

void network_game_session::handle_host_message(
    const network_message& message
) {
    if (const auto* hello = std::get_if<hello_message>(&message)) {
        if (peer_hello_received_) {
            fail_session("Duplicate HELLO message", true);
            return;
        }
        if (hello->version != network_protocol_version) {
            fail_session("Incompatible protocol version", true);
            return;
        }

        peer_hello_received_ = true;
        if (!send_message(role_message{ piece_side::Black })) return;
        if (!send_current_state()) return;
        state_ = network_session_state::playing;
        status_text_ = "Connected - you are Red";
        return;
    }

    if (const auto* move = std::get_if<move_message>(&message)) {
        if (state_ != network_session_state::playing ||
            !peer_hello_received_) {
            fail_session("MOVE received before handshake completed", true);
            return;
        }
        if (game_.current_turn() != piece_side::Black) {
            reject_client_move("Client attempted to move out of turn");
            return;
        }
        if (move->sequence != game_.sequence()) {
            reject_client_move("Client used an outdated state sequence");
            return;
        }
        if (!game_.try_move(move->move, rules_)) {
            reject_client_move("Client submitted an illegal move");
            return;
        }

        status_text_ = "Connected - you are Red";
        send_current_state();
        return;
    }

    if (std::holds_alternative<quit_message>(message)) {
        transport_.stop();
        state_ = network_session_state::disconnected;
        status_text_ = "The other player left the game";
        return;
    }

    if (std::holds_alternative<restart_message>(message)) {
        if (state_ != network_session_state::playing ||
            game_.status() == game_status::goingOn) {
            reject_client_move("Restart is only allowed after game over");
            return;
        }
        game_.reset();
        if (!send_message(restart_message{})) return;
        if (!send_current_state()) return;
        status_text_ = "Connected - you are Red";
        return;
    }

    if (const auto* error = std::get_if<error_message>(&message)) {
        status_text_ = "Peer error: " + error->reason;
        return;
    }

    fail_session("Host received a message not allowed from the client", true);
}

void network_game_session::handle_client_message(
    const network_message& message
) {
    if (const auto* hello = std::get_if<hello_message>(&message)) {
        if (peer_hello_received_) {
            fail_session("Duplicate HELLO message", true);
            return;
        }
        if (hello->version != network_protocol_version) {
            fail_session("Incompatible protocol version", true);
            return;
        }
        peer_hello_received_ = true;
        return;
    }

    if (const auto* assigned_role = std::get_if<role_message>(&message)) {
        if (!peer_hello_received_ || assigned_role_received_ ||
            assigned_role->side != piece_side::Black) {
            fail_session("Host assigned an invalid role", true);
            return;
        }
        assigned_role_received_ = true;
        return;
    }

    if (const auto* state = std::get_if<state_message>(&message)) {
        if (!peer_hello_received_ || !assigned_role_received_) {
            fail_session("STATE received before handshake completed", true);
            return;
        }
        if (initial_state_received_ && state->sequence < game_.sequence()) {
            fail_session("Host sent an outdated state", true);
            return;
        }

        game_.apply_snapshot(
            state->board,
            state->current_turn,
            state->status,
            state->sequence,
            state->last_move
        );
        initial_state_received_ = true;
        move_pending_ = false;
        restart_pending_ = false;
        state_ = network_session_state::playing;
        status_text_ = "Connected - you are Black";
        return;
    }

    if (const auto* error = std::get_if<error_message>(&message)) {
        move_pending_ = false;
        status_text_ = "Host rejected request: " + error->reason;
        return;
    }

    if (std::holds_alternative<quit_message>(message)) {
        transport_.stop();
        state_ = network_session_state::disconnected;
        status_text_ = "The host ended the game";
        move_pending_ = false;
        restart_pending_ = false;
        return;
    }

    if (std::holds_alternative<restart_message>(message)) {
        if (game_.status() == game_status::goingOn && !restart_pending_) {
            fail_session("Unexpected RESTART message", true);
            return;
        }
        game_.reset();
        restart_pending_ = false;
        move_pending_ = false;
        status_text_ = "Connected - you are Black";
        return;
    }

    fail_session("Client received a message not allowed from the host", true);
}

bool network_game_session::send_message(const network_message& message) {
    std::string error;
    if (transport_.send_text(serialize_message(message), error)) {
        return true;
    }
    state_ = network_session_state::failed;
    status_text_ = error;
    move_pending_ = false;
    restart_pending_ = false;
    return false;
}

bool network_game_session::send_current_state() {
    return send_message(state_message{
        game_.sequence(),
        game_.current_turn(),
        game_.status(),
        game_.last_move(),
        game_.board()
    });
}

void network_game_session::reject_client_move(std::string reason) {
    send_message(error_message{ std::move(reason) });
    if (state_ != network_session_state::failed) {
        send_current_state();
    }
}

void network_game_session::fail_session(
    std::string reason,
    bool notify_peer
) {
    if (notify_peer &&
        transport_.state() == network_connection_state::connected) {
        std::string ignored_error;
        transport_.send_text(
            serialize_message(error_message{ reason }),
            ignored_error
        );
    }
    transport_.stop();
    state_ = network_session_state::failed;
    status_text_ = std::move(reason);
    move_pending_ = false;
    restart_pending_ = false;
}
