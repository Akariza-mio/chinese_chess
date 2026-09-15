#include <cmath>
#include "board_painter.hpp"
#include "move_animation.hpp"
#include "raylib.h"

namespace {

void draw_centered_text(
    Font font,
    const char* text,
    Rectangle area,
    float font_size,
    Color color
) {
    const Vector2 size = MeasureTextEx(font, text, font_size, 0.0f);
    const Vector2 position{
        area.x + (area.width - size.x) / 2.0f,
        area.y + (area.height - size.y) / 2.0f
    };
    DrawTextEx(font, text, position, font_size, 0.0f, color);
}

void draw_menu_button(Font font, Rectangle area, const char* text) {
    const bool hovered = CheckCollisionPointRec(GetMousePosition(), area);
    DrawRectangleRec(area, hovered ? LIGHTGRAY : RAYWHITE);
    DrawRectangleLinesEx(area, 3.0f, DARKGRAY);
    draw_centered_text(font, text, area, 38.0f, DARKGRAY);
}

} // namespace

board_painter::board_painter() {
    const char* all_chars =
        "楚河汉界将帅士仕相象马车炮兵卒红黑方胜当前行棋被军重开正在：！"
        "中国棋本地对局创建联机加入输入主机地址按回车连接等待玩家你的"
        "局域网端口返回菜单请求已发送确认失败断开握手取消执，再错误";
    int cnt = 0;
    int* codepoint = LoadCodepoints(all_chars, &cnt);
    chinese_font = LoadFontEx("assets/fonts/NotoSansCJKsc-Regular.otf", 48, codepoint, cnt);
    UnloadCodepoints(codepoint);
}
board_painter::~board_painter() {
    UnloadFont(chinese_font);
}
//画炮兵位的记号
void board_painter::draw_jin(int x,int y,int side) const {
    if (side == 0) {
        for (int i = 0; i < 2; ++i) {
            DrawLineEx(
                Vector2{ (float)x - 17 ,(float)y - 5 + i * 10 },
                Vector2{ (float)x - 7 ,(float)y - 5 + i * 10 },
                2.0f, DARKGRAY
            );
        }
        for (int i = 0; i < 2; ++i) {
            DrawLineEx(
                Vector2{ (float)x - 7 ,(float)y - 15 + i * 20 },
                Vector2{ (float)x - 7 ,(float)y - 5 + i * 20 },
                2.0f, DARKGRAY
            );
        }
        return;
    }
    if (side == 1) {
        for (int i = 0; i < 2; ++i) {
            DrawLineEx(
                Vector2{ (float)x + 17 ,(float)y - 5 + i * 10 },
                Vector2{ (float)x + 7 ,(float)y - 5 + i * 10 },
                2.0f, DARKGRAY
            );
        }
        for (int i = 0; i < 2; ++i) {
            DrawLineEx(
                Vector2{ (float)x + 7 ,(float)y - 15 + i * 20 },
                Vector2{ (float)x + 7 ,(float)y - 5 + i * 20 },
                2.0f, DARKGRAY
            );
        }
        return;
    }
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            DrawLineEx(
                Vector2{ (float)x - 15 + j * 20,(float)y - 5 + i * 10},
                Vector2{ (float)x - 5 + j * 20,(float)y - 5 + i * 10},
                2.0f, DARKGRAY
            );
        }
    }
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            DrawLineEx(
                Vector2{ (float)x - 5 + j * 10,(float)y - 15 + i * 20 },
                Vector2{ (float)x - 5 + j * 10,(float)y - 5 + i * 20 },
                2.0f, DARKGRAY
            );
        }
    }
}
//绘制棋盘
void board_painter::draw_board(bool flipped) const{
    DrawRectangleLinesEx(
        Rectangle(original_x, original_y, chess_width, chess_height),
        4.0f, DARKGRAY
    );
    for (int i = 1; i <= 8; ++i) {
        DrawLineEx(
            Vector2{ original_x,(float)original_y + i * grid_wid },
            Vector2{ original_x + chess_width,(float)original_y + i * grid_wid },
            2.0f, DARKGRAY
        );
    }
    for (int i = 1; i <= 7; ++i) {
        DrawLineEx(
            Vector2{ (float)original_x + i * grid_wid,original_y },
            Vector2{ (float)original_x + i * grid_wid,original_y + chess_width / 2 },
            2.0f, DARKGRAY
        );
    }
    for (int i = 1; i <= 7; ++i) {
        DrawLineEx(
            Vector2{ (float)original_x + i * grid_wid,260.0f + chess_width / 2 },
            Vector2{ (float)original_x + i * grid_wid,original_y + chess_height },
            2.0f, DARKGRAY
        );
    }
    for (int i = 0; i < 2; ++i) {
        DrawLineEx(
            Vector2{ (float)original_x + 3 * grid_wid,(float)original_y + i * 560 },
            Vector2{ (float)original_x + 5 * grid_wid,(float)original_y + i * 560 + 160 },
            2.0f, DARKGRAY
        );
    }
    for (int i = 0; i < 2; ++i) {
        DrawLineEx(
            Vector2{ (float)original_x + 5 * grid_wid,(float)original_y + i * 560 },
            Vector2{ (float)original_x + 3 * grid_wid,(float)original_y + i * 560 + 160 },
            2.0f, DARKGRAY
        );
    }
    const char* left_river_text = flipped ? "汉界" : "楚河";
    const char* right_river_text = flipped ? "楚河" : "汉界";
    DrawTextEx(chinese_font, left_river_text, Vector2{ 200.0f, 515.0f }, 48.0f, 4.0f, DARKGRAY);
    DrawTextEx(chinese_font, right_river_text, Vector2{ 520.0f, 515.0f }, 48.0f, 4.0f, DARKGRAY);
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            draw_jin(original_x + grid_wid + j * grid_wid * 6,
                original_y + 2 * grid_wid + i * grid_wid * 5,2);
        }
    }
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            draw_jin(original_x + 2 * grid_wid + j * grid_wid * 2,
                original_y + 3 * grid_wid + i * grid_wid * 3,2);
        }
    }
    draw_jin(original_x, original_y + 3 * grid_wid, 1);
    draw_jin(original_x, original_y + 6 * grid_wid, 1);
    draw_jin(original_x + chess_width, original_y + 3 * grid_wid, 0);
    draw_jin(original_x + chess_width, original_y + 6 * grid_wid, 0);
}
const char* board_painter::get_piece_text(piece_type type, piece_side side)const {
    switch (type) {
    case piece_type::General: return (side == piece_side::Black) ? "将" : "帅";
    case piece_type::Shi:     return (side == piece_side::Black) ? "士" : "仕";
    case piece_type::Xiang:   return (side == piece_side::Black) ? "象" : "相";
    case piece_type::Ma:      return "马";
    case piece_type::Ju:      return "车";
    case piece_type::Pao:     return "炮";
    case piece_type::Bing:    return (side == piece_side::Black) ? "卒" : "兵";
    }
    return "";
}
void board_painter::draw_piece_at(
    const piece& value,
    float center_x,
    float center_y
) const {
    const Color color = value.get_side() == piece_side::Red ? RED : BLACK;
    DrawCircleV(Vector2{ center_x, center_y }, 35.0f, color);
    const char* text = get_piece_text(value.get_type(), value.get_side());
    DrawTextEx(
        chinese_font,
        text,
        Vector2{ center_x - 17.0f, center_y - 25.0f },
        50.0f,
        0.0f,
        WHITE
    );
}

void board_painter::draw_pieces(
    const chess_board& board,
    bool flipped,
    std::optional<pos> hidden_position
) const {
    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < 9; ++col) {
            pos now_pos{row,col};
            if (hidden_position.has_value() && now_pos == *hidden_position) {
                continue;
            }
            const std::optional<piece>& cell = board.at(now_pos);
            if (!cell.has_value()) {
                continue;
            }
            const pos shown = orient_board_position(now_pos, flipped);
            const float center_x = static_cast<float>(original_x + shown.col * grid_wid);
            const float center_y = static_cast<float>(original_y + shown.row * grid_wid);
            draw_piece_at(*cell, center_x, center_y);
        }
    }
}

void board_painter::draw_moving_piece(
    const move_animation& animation,
    bool flipped
) const {
    if (!animation.active()) return;

    const pos shown_from = orient_board_position(animation.move().from, flipped);
    const pos shown_to = orient_board_position(animation.move().to, flipped);
    const float progress = animation.progress();
    const float from_x = static_cast<float>(original_x + shown_from.col * grid_wid);
    const float from_y = static_cast<float>(original_y + shown_from.row * grid_wid);
    const float to_x = static_cast<float>(original_x + shown_to.col * grid_wid);
    const float to_y = static_cast<float>(original_y + shown_to.row * grid_wid);

    if (animation.captured_piece().has_value()) {
        draw_piece_at(*animation.captured_piece(), to_x, to_y);
    }
    draw_piece_at(
        animation.moving_piece(),
        from_x + (to_x - from_x) * progress,
        from_y + (to_y - from_y) * progress
    );
}
void board_painter::draw_selected_sign(const pos& p, bool flipped)const {
    const pos shown = orient_board_position(p, flipped);
    int center_x = shown.col * grid_wid + original_x;
    int center_y = shown.row * grid_wid + original_y;
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            DrawLineEx(
                Vector2{ (float)center_x - 35 + j * 60,(float)center_y - 35 + i * 70},
                Vector2{ (float)center_x - 25 + j * 60,(float)center_y - 35 + i * 70},
                2.0f, BLUE
            );
        }
    }
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            DrawLineEx(
                Vector2{ (float)center_x - 35 + j * 70,(float)center_y - 35 + i * 60 },
                Vector2{ (float)center_x - 35 + j * 70,(float)center_y - 25 + i * 60 },
                2.0f, BLUE
            );
        }
    }
}
std::optional<pos> board_painter::get_pos_from_mouse(
    int mouse_x,
    int mouse_y,
    bool flipped
)const {
    float col_mouse = (mouse_x - original_x) / (float)grid_wid;
    float row_mouse = (mouse_y - original_y) / (float)grid_wid;
    int col = static_cast<int>(std::round(col_mouse));
    int row = static_cast<int>(std::round(row_mouse));
    if (row > 9 || row < 0 || col > 8 || col < 0) {
        return std::nullopt;
    }
    int center_x = original_x + col * grid_wid;
    int center_y = original_y + row * grid_wid;
    int dx = mouse_x - center_x;
    int dy = mouse_y - center_y;
    if (dx * dx + dy * dy <= 35 * 35) {
        const pos shown{ row, col };
        return orient_board_position(shown, flipped);
    }
    return std::nullopt;
}
void board_painter::draw_status(piece_side now_turn, bool is_checking)const {
    const char* turn_text = "";
    Color c1;
    if (now_turn == piece_side::Red) {
        turn_text = "当前行棋：红方";
        c1 = RED;
    }
    else {
        turn_text = "当前行棋：黑方"; 
        c1 = BLACK;
    }
    DrawTextEx(chinese_font, turn_text, Vector2{ 20.0f, 100.0f }, 40.0f, 0, c1);
    if (is_checking) {
        const char* check_text = "";
        Color c2;
        if (now_turn == piece_side::Red) {
            check_text = "红方正在被将军！";
            c2 = RED;
        }
        else {
            check_text = "黑方正在被将军！";
            c2 = BLACK;
        }
        DrawTextEx(chinese_font, check_text, Vector2{ 20.0f, 40.0f }, 48.0f, 0, c2);
    }
}
void board_painter::draw_game_over_window(
    game_status status,
    const char* button_text
) const {
    if (status == game_status::goingOn) return;
    DrawRectangle(0, 0, 800, 1000, Color{ 0, 0, 0, 150 });
    DrawRectangle(200, 350, 400, 300, RAYWHITE);
    DrawRectangleLines(200, 350, 400, 300, DARKGRAY);
    const char* win_text = (status == game_status::RedWin_checkmate || status == game_status::RedWin_stalemate) ? "红方胜" : "黑方胜";
    Color c = (status == game_status::RedWin_checkmate || status == game_status::RedWin_stalemate) ? RED : BLACK;
    DrawTextEx(chinese_font, win_text, Vector2{ 340.0f, 400.0f }, 60.0f, 0, c);
    // 重开按钮
    Rectangle restart_area = { 300, 520, 200, 80 };
    DrawRectangleRec(restart_area, LIGHTGRAY);
    DrawRectangleLinesEx(restart_area, 2.0f, DARKGRAY);
    draw_centered_text(chinese_font, button_text, restart_area, 42.0f, BLACK);
}

void board_painter::draw_main_menu() const {
    draw_centered_text(
        chinese_font,
        "中国象棋",
        Rectangle{ 0.0f, 150.0f, 800.0f, 100.0f },
        64.0f,
        DARKGRAY
    );
    draw_menu_button(
        chinese_font,
        Rectangle{ 250.0f, 350.0f, 300.0f, 70.0f },
        "本地对局"
    );
    draw_menu_button(
        chinese_font,
        Rectangle{ 250.0f, 450.0f, 300.0f, 70.0f },
        "创建联机"
    );
    draw_menu_button(
        chinese_font,
        Rectangle{ 250.0f, 550.0f, 300.0f, 70.0f },
        "加入联机"
    );
}

void board_painter::draw_join_screen(
    const std::string& ip,
    const std::string& message
) const {
    draw_centered_text(
        chinese_font,
        "输入主机地址",
        Rectangle{ 0.0f, 220.0f, 800.0f, 80.0f },
        48.0f,
        DARKGRAY
    );
    const Rectangle input_area{ 170.0f, 350.0f, 460.0f, 70.0f };
    DrawRectangleRec(input_area, RAYWHITE);
    DrawRectangleLinesEx(input_area, 3.0f, DARKGRAY);
    DrawText(ip.c_str(), 190, 370, 32, BLACK);
    draw_centered_text(
        chinese_font,
        "按回车连接",
        Rectangle{ 0.0f, 440.0f, 800.0f, 60.0f },
        32.0f,
        DARKGRAY
    );
    if (!message.empty()) {
        DrawText(message.c_str(), 80, 520, 22, MAROON);
    }
    draw_back_button();
}

void board_painter::draw_connection_screen(
    const std::string& message,
    const std::vector<std::string>& local_addresses
) const {
    draw_centered_text(
        chinese_font,
        "等待玩家连接",
        Rectangle{ 0.0f, 180.0f, 800.0f, 100.0f },
        52.0f,
        DARKGRAY
    );
    draw_centered_text(
        chinese_font,
        "你的局域网地址",
        Rectangle{ 0.0f, 300.0f, 800.0f, 60.0f },
        34.0f,
        DARKGRAY
    );

    int y = 380;
    if (local_addresses.empty()) {
        DrawText("No LAN IPv4 address found", 220, y, 24, MAROON);
        y += 45;
    }
    else {
        for (const std::string& address : local_addresses) {
            DrawText(address.c_str(), 300, y, 32, BLACK);
            y += 45;
        }
    }
    DrawText("Port: 27015", 315, y + 15, 26, DARKGRAY);
    if (!message.empty()) {
        DrawText(message.c_str(), 80, y + 80, 22, DARKGRAY);
    }
    draw_back_button();
}

void board_painter::draw_message_screen(
    const char* title,
    const std::string& message
) const {
    draw_centered_text(
        chinese_font,
        title,
        Rectangle{ 0.0f, 240.0f, 800.0f, 90.0f },
        52.0f,
        DARKGRAY
    );
    if (!message.empty()) {
        DrawText(message.c_str(), 80, 380, 22, MAROON);
    }
    draw_back_button();
}

void board_painter::draw_back_button() const {
    draw_menu_button(
        chinese_font,
        Rectangle{ 600.0f, 70.0f, 180.0f, 55.0f },
        "返回菜单"
    );
}
