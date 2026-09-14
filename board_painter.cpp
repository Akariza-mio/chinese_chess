#include <cmath>
#include "board_painter.hpp"
#include "raylib.h"

board_painter::board_painter() {
    const char* all_chars = "楚河汉界将帅士仕相象马车炮兵卒红黑方胜当前行棋被军重开正在：！";
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
void board_painter::draw_board() const{
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
    DrawTextEx(chinese_font, "楚河", Vector2{ 200.0f, 515.0f }, 48.0f, 4.0f, DARKGRAY);
    DrawTextEx(chinese_font, "汉界", Vector2{ 520.0f, 515.0f }, 48.0f, 4.0f, DARKGRAY);
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
void board_painter::draw_pieces(const chess_board& board) const {
    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < 9; ++col) {
            pos now_pos{row,col};
            const std::optional<piece>& cell = board.at(now_pos);
            if (!cell.has_value()) {
                continue;
            }
            int center_x = original_x + col * grid_wid;
            int center_y = original_y + row * grid_wid;
            Color c = (cell->get_side() == piece_side::Red) ? RED : BLACK;
            DrawCircle(center_x, center_y, 35.0f, c);
            const char* text = get_piece_text(cell->get_type(), cell->get_side());
            Vector2 text_pos = {
                (float)center_x - 17.0f,
                (float)center_y - 25.0f
            };
            DrawTextEx(chinese_font, text, text_pos, 50.0f, 0, WHITE);

        }
    }
}
void board_painter::draw_selected_sign(const pos& p)const {
    int center_x = p.col * grid_wid + original_x;
    int center_y = p.row * grid_wid + original_y;
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
std::optional<pos> board_painter::get_pos_from_mouse(int mouse_x, int mouse_y)const {
    float col_mouse = (mouse_x - original_x) / (float)grid_wid;
    float row_mouse = (mouse_y - original_y) / (float)grid_wid;
    int col = std::round(col_mouse);
    int row = std::round(row_mouse);
    if (row > 9 || row < 0 || col > 8 || col < 0) {
        return std::nullopt;
    }
    int center_x = original_x + col * grid_wid;
    int center_y = original_y + row * grid_wid;
    int dx = mouse_x - center_x;
    int dy = mouse_y - center_y;
    if (dx * dx + dy * dy <= 35 * 35) {
        return pos{ row,col };
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
void board_painter::draw_game_over_window(game_status status) const {
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
    DrawTextEx(chinese_font, "重开", Vector2{ 370.0f, 535.0f }, 48.0f, 0, BLACK);
}