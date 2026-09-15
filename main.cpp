#include <optional>
#include <string>
#include <vector>

#include "raylib.h"
#include "board_painter.hpp"
#include "game_state.hpp"
#include "move_animation.hpp"
#include "network_game_session.hpp"
#include "rule_engine.hpp"

constexpr int window_width = 800;
constexpr int window_height = 1000;

constexpr Rectangle local_button{ 250.0f, 350.0f, 300.0f, 70.0f };
constexpr Rectangle host_button{ 250.0f, 450.0f, 300.0f, 70.0f };
constexpr Rectangle join_button{ 250.0f, 550.0f, 300.0f, 70.0f };
constexpr Rectangle back_button{ 600.0f, 70.0f, 180.0f, 55.0f };
constexpr Rectangle game_over_button{ 300.0f, 520.0f, 200.0f, 80.0f };

enum class app_page {
    main_menu,
    local_game,
    join_input,
    network_host,
    network_client
};

bool left_clicked(Rectangle area) {
    return IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
        CheckCollisionPointRec(GetMousePosition(), area);
}

void handle_local_click(
    game_state& game,
    std::optional<pos>& selected_pos,
    board_painter& painter,
    const rule_engine& rules,
    move_animation& animation
) {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;

    const std::optional<pos> click_pos = painter.get_pos_from_mouse(
        GetMouseX(), GetMouseY()
    );
    if (!click_pos.has_value()) {
        selected_pos.reset();
        return;
    }

    const std::optional<piece>& click_cell = game.board().at(*click_pos);
    if (!selected_pos.has_value()) {
        if (click_cell.has_value() &&
            click_cell->get_side() == game.current_turn()) {
            selected_pos = click_pos;
        }
        return;
    }

    if (click_cell.has_value() &&
        click_cell->get_side() == game.current_turn()) {
        selected_pos = click_pos;
        return;
    }

    const a_move move{ *selected_pos, *click_pos };
    const chess_board board_before_move = game.board();
    if (game.try_move(move, rules)) {
        animation.start(board_before_move, move);
    }
    selected_pos.reset();
}

void handle_network_click(
    network_game_session& session,
    std::optional<pos>& selected_pos,
    board_painter& painter,
    bool flipped,
    move_animation& animation
) {
    if (!session.can_local_move()) {
        selected_pos.reset();
        return;
    }
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;

    const std::optional<pos> click_pos = painter.get_pos_from_mouse(
        GetMouseX(), GetMouseY(), flipped
    );
    if (!click_pos.has_value()) {
        selected_pos.reset();
        return;
    }

    const std::optional<piece_side> local_side = session.local_side();
    const std::optional<piece>& click_cell = session.game().board().at(*click_pos);
    if (!selected_pos.has_value()) {
        if (local_side.has_value() && click_cell.has_value() &&
            click_cell->get_side() == *local_side) {
            selected_pos = click_pos;
        }
        return;
    }

    if (local_side.has_value() && click_cell.has_value() &&
        click_cell->get_side() == *local_side) {
        selected_pos = click_pos;
        return;
    }

    std::string error;
    const a_move move{ *selected_pos, *click_pos };
    const chess_board board_before_move = session.game().board();
    const std::uint32_t sequence_before_move = session.game().sequence();
    if (session.submit_local_move(move, error) &&
        session.game().sequence() != sequence_before_move) {
        animation.start(board_before_move, move);
    }
    selected_pos.reset();
}

void update_ip_input(std::string& ip) {
    int codepoint = GetCharPressed();
    while (codepoint > 0) {
        const bool allowed = (codepoint >= '0' && codepoint <= '9') ||
            codepoint == '.';
        if (allowed && ip.size() < 15) {
            ip.push_back(static_cast<char>(codepoint));
        }
        codepoint = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && !ip.empty()) {
        ip.pop_back();
    }
}

int main() {
    InitWindow(window_width, window_height, "Chinese Chess");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    board_painter* painter = new board_painter();
    rule_engine* rules = new rule_engine();
    game_state local_game;
    network_game_session network_session;
    move_animation animation;

    app_page page = app_page::main_menu;
    std::optional<pos> selected_pos;
    std::string join_ip = "127.0.0.1";
    std::string join_message;
    std::vector<std::string> local_addresses;

    while (!WindowShouldClose()) {
        animation.update(GetFrameTime());
        if (page == app_page::network_host ||
            page == app_page::network_client) {
            const chess_board board_before_update = network_session.game().board();
            const std::uint32_t sequence_before_update =
                network_session.game().sequence();
            network_session.update();
            const game_state& updated_game = network_session.game();
            if (updated_game.sequence() != sequence_before_update) {
                if (updated_game.sequence() > sequence_before_update &&
                    updated_game.last_move().has_value()) {
                    animation.start(
                        board_before_update,
                        *updated_game.last_move()
                    );
                }
                else {
                    animation.reset();
                }
            }
        }

        if (IsKeyPressed(KEY_ESCAPE) && page != app_page::main_menu) {
            if (page == app_page::network_host ||
                page == app_page::network_client) {
                network_session.stop();
            }
            selected_pos.reset();
            animation.reset();
            page = app_page::main_menu;
        }

        if (page == app_page::main_menu) {
            if (left_clicked(local_button)) {
                local_game.reset();
                selected_pos.reset();
                animation.reset();
                page = app_page::local_game;
            }
            else if (left_clicked(host_button)) {
                std::string error;
                local_addresses = get_local_ipv4_addresses();
                network_session.host(error);
                selected_pos.reset();
                animation.reset();
                page = app_page::network_host;
            }
            else if (left_clicked(join_button)) {
                join_message.clear();
                page = app_page::join_input;
            }
        }
        else if (page == app_page::join_input) {
            update_ip_input(join_ip);
            if (left_clicked(back_button)) {
                page = app_page::main_menu;
            }
            else if (IsKeyPressed(KEY_ENTER)) {
                std::string error;
                if (network_session.join(join_ip, error)) {
                    selected_pos.reset();
                    animation.reset();
                    page = app_page::network_client;
                }
                else {
                    join_message = error;
                }
            }
        }
        else if (page == app_page::local_game) {
            if (local_game.status() == game_status::goingOn) {
                if (!animation.active()) {
                    handle_local_click(
                        local_game,
                        selected_pos,
                        *painter,
                        *rules,
                        animation
                    );
                }
                if (left_clicked(back_button)) {
                    selected_pos.reset();
                    animation.reset();
                    page = app_page::main_menu;
                }
            }
            else if (!animation.active() && left_clicked(game_over_button)) {
                local_game.reset();
                selected_pos.reset();
                animation.reset();
            }
        }
        else {
            const bool is_client = page == app_page::network_client;
            const bool game_was_over_before_click =
                network_session.game().status() != game_status::goingOn;
            if (network_session.state() == network_session_state::playing &&
                !animation.active()) {
                handle_network_click(
                    network_session,
                    selected_pos,
                    *painter,
                    is_client,
                    animation
                );
            }
            else {
                selected_pos.reset();
            }

            if (left_clicked(back_button)) {
                network_session.stop();
                selected_pos.reset();
                animation.reset();
                page = app_page::main_menu;
            }
            else if (!animation.active() && game_was_over_before_click &&
                left_clicked(game_over_button)) {
                std::string error;
                network_session.request_restart(error);
                selected_pos.reset();
                animation.reset();
            }
        }

        BeginDrawing();
        ClearBackground(YELLOW);

        if (page == app_page::main_menu) {
            painter->draw_main_menu();
        }
        else if (page == app_page::join_input) {
            painter->draw_join_screen(join_ip, join_message);
        }
        else if ((page == app_page::network_host ||
            page == app_page::network_client) &&
            network_session.state() != network_session_state::playing) {
            const network_session_state session_state = network_session.state();
            if (page == app_page::network_host &&
                (session_state == network_session_state::hosting ||
                 session_state == network_session_state::handshaking)) {
                painter->draw_connection_screen(
                    network_session.status_text(), local_addresses
                );
            }
            else {
                const char* title =
                    (session_state == network_session_state::connecting ||
                     session_state == network_session_state::handshaking)
                    ? "正在连接"
                    : "连接失败";
                painter->draw_message_screen(
                    title, network_session.status_text()
                );
            }
        }
        else {
            const bool is_network = page == app_page::network_host ||
                page == app_page::network_client;
            const bool flipped = page == app_page::network_client;
            const game_state& shown_game = is_network
                ? network_session.game()
                : local_game;

            painter->draw_board(flipped);
            painter->draw_pieces(
                shown_game.board(),
                flipped,
                animation.active()
                    ? std::optional<pos>{ animation.move().to }
                    : std::nullopt
            );
            painter->draw_moving_piece(animation, flipped);
            if (selected_pos.has_value() && !animation.active()) {
                painter->draw_selected_sign(*selected_pos, flipped);
            }

            const bool checking = rules->is_in_check(
                shown_game.board(), shown_game.current_turn()
            );
            painter->draw_status(shown_game.current_turn(), checking);

            if (!is_network &&
                shown_game.status() == game_status::goingOn) {
                painter->draw_back_button();
            }

            if (shown_game.status() != game_status::goingOn &&
                !animation.active()) {
                painter->draw_game_over_window(
                    shown_game.status(),
                    is_network
                        ? (network_session.restart_pending()
                            ? "等待主机"
                            : "重开")
                        : "重开"
                );
            }
            if (is_network) {
                painter->draw_back_button();
            }
        }

        EndDrawing();
    }

    network_session.stop();
    delete painter;
    delete rules;
    CloseWindow();
}
