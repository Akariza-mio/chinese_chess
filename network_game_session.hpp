#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "game_state.hpp"
#include "network_protocol.hpp"
#include "network_transport.hpp"

enum class network_session_state {
    idle,
    hosting,
    connecting,
    handshaking,
    playing,
    disconnected,
    failed
};

class network_game_session {
public:
    network_game_session() = default;
    ~network_game_session() = default;

    network_game_session(const network_game_session&) = delete;
    network_game_session& operator=(const network_game_session&) = delete;

    bool host(
        std::string& error,
        std::uint16_t port = default_chess_port
    );
    bool join(
        std::string_view ipv4_address,
        std::string& error,
        std::uint16_t port = default_chess_port
    );

    // raylib 主线程每帧调用一次，处理网络线程送来的事件。
    void update();

    // 主机在本地验证红方着法；客户端只提交黑方请求，等待主机 STATE。
    bool submit_local_move(const a_move& move, std::string& error);
    bool request_restart(std::string& error);
    bool request_undo(std::string& error);
    bool respond_to_undo(bool accepted, std::string& error);

    void stop();

    const game_state& game() const;
    network_session_state state() const;
    std::optional<piece_side> local_side() const;
    bool can_local_move() const;
    bool move_pending() const;
    bool restart_pending() const;
    bool can_request_undo() const;
    bool undo_request_pending() const;
    bool undo_request_incoming() const;
    const std::string& status_text() const;

private:
    enum class role {
        none,
        host_red,
        client_black
    };

    struct move_record {
        piece_side mover;
        game_state before_move;
    };

    void handle_transport_event(const network_event& event);
    void handle_line(std::string_view line);
    void handle_host_message(const network_message& message);
    void handle_client_message(const network_message& message);

    bool send_message(const network_message& message);
    bool send_current_state();
    void reject_client_move(std::string reason);
    void fail_session(std::string reason, bool notify_peer);
    bool has_undo_point(piece_side side) const;
    bool apply_undo(piece_side side);
    void clear_undo_requests();
    void clear_history();

    network_transport transport_;
    game_state game_;
    rule_engine rules_;
    role role_ = role::none;
    network_session_state state_ = network_session_state::idle;
    std::string status_text_ = "Not connected";
    bool peer_hello_received_ = false;
    bool assigned_role_received_ = false;
    bool initial_state_received_ = false;
    bool move_pending_ = false;
    bool restart_pending_ = false;
    bool undo_request_pending_ = false;
    bool undo_request_incoming_ = false;
    bool red_can_undo_ = false;
    bool black_can_undo_ = false;
    std::vector<move_record> move_history_;
};
