#include "fltkio.hpp"
#include <Fl/Fl.H>
#include <Fl/Fl_Window.H>
#include <Fl/fl_draw.H>
#include <Fl/Fl_Button.H>
#include <Fl/Fl_Box.H>
#include <Fl/Fl_Browser.H>
#include <Fl/fl_ask.H>
#include <iostream>
#include <string>
#include <cstring>

FltkIO::FltkIO() {
    window = new Fl_Window(600, 420, "Checkers");

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            int x = 200 + c * 50;
            int y = r * 50;
            
            board_buttons[r][c] = new Fl_Button(x, y, 50, 50);
            board_buttons[r][c]->box(FL_FLAT_BOX);
            
            board_buttons[r][c]->callback(cb_board_click, this);

            if ((r + c) % 2 == 0) {
                board_buttons[r][c]->color(FL_GRAY);
            } else {
                board_buttons[r][c]->color(FL_WHITE);
                board_buttons[r][c]->deactivate();
            }
        }
    }

    for (int i = 0; i < 8; ++i) {
        Fl_Box* lbl = new Fl_Box(180, i * 50 + 15, 20, 20);
        char buf[2];
        snprintf(buf, sizeof(buf), "%d", 8 - i);
        lbl->copy_label(buf);
    }

    for (int i = 0; i < 8; ++i) {
        Fl_Box* lbl = new Fl_Box(200 + i * 50 + 12, 400, 25, 20);
        char buf[2];
        buf[0] = 'A' + i;
        buf[1] = '\0';
        lbl->copy_label(buf);
    }

    btn_new_game = new Fl_Button(5, 5, 175, 40, "New Game");
    btn_new_game->callback(cb_new_game, this);

    btn_end_game = new Fl_Button(5, 50, 175, 40, "End Game");
    btn_end_game->callback(cb_end_game, this);

    btn_quit = new Fl_Button(5, 95, 175, 40, "Quit Program");
    btn_quit->callback(cb_quit, this);

    history_view = new Fl_Browser(5, 140, 175, 275, "Move History");
    history_view->box(FL_DOWN_BOX);
    history_view->color(FL_WHITE);

    window->end();
    window->show();

    input_state.waiting_for_move = false;
    input_state.selected_r = -1;
    current_board_ptr = nullptr;
    input_state.new_game_requested = false;
}

FltkIO::~FltkIO() {
    delete window;
}

void FltkIO::displayBoard(const Board& board, const char* status_msg, const char* error_msg)
{
    if (status_msg) window->label(status_msg);
    current_board_ptr = &board;
    
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Fl_Button* btn = board_buttons[r][c];
            
            if ((r + c) % 2 == 0) {
                btn->color(FL_GRAY);
            } else {
                btn->color(FL_WHITE);
            }
            
            int b_r = 7 - r;
            int b_c = c;
            const Piece* piece = board.getPiece({b_r, b_c});
            
            btn->label("");
            if (piece) {
                btn->copy_label(piece->getColor() == RED ? (piece->getKing() ? "RK" : "R") : (piece->getKing() ? "BK" : "B"));
                btn->labelcolor(piece->getColor() == RED ? FL_RED : FL_BLACK);
            }
            btn->redraw();
        }
    }
    
    if (error_msg && strlen(error_msg) > 0 && strcmp(error_msg, "No error.") != 0) {
        fl_alert("%s", error_msg);
    }
    
    Fl::flush();
}

bool FltkIO::getPlayerMove(Color player, Move* move_out)
{
    input_state.current_player = player;
    input_state.move_out_ptr = move_out;
    input_state.waiting_for_move = true;
    input_state.move_completed = false;
    input_state.selected_r = -1;
    input_state.selected_c = -1;

    while (input_state.waiting_for_move) {
        if (Fl::wait() == 0) {
            return false;
        }
        
        if (input_state.move_completed) {
            return true;
        }
    }
    return false;
}

void FltkIO::displayHistory(const std::vector<Move>& history)
{
    history_view->clear();
    for (const auto& move : history) {
        char buf[128];
        char color_code = (move.player_color == RED) ? (char)FL_RED : (char)FL_BLACK;
        snprintf(buf, sizeof(buf), "@C%c%s", color_code, move.to_char());
        history_view->add(buf);
    }
    if (history_view->size() > 0) {
        history_view->bottomline(history_view->size());
    }
}

void FltkIO::waitForContinuation() {
    input_state.new_game_requested = false;
    while (!input_state.new_game_requested) {
        if (Fl::wait() == 0) exit(0);
    }
    input_state.new_game_requested = false;
}

void FltkIO::processEvents() {
    Fl::check();
}

void FltkIO::handle_board_click(int r, int c) {
    if (!input_state.waiting_for_move) return;

    int b_r = 7 - r;
    int b_c = c;
    
    std::cout << "DEBUG: Clicked UI(" << r << "," << c << ") -> Board(" << b_r << "," << b_c << ")" << std::endl;

    if (input_state.selected_r == -1) {
        if (current_board_ptr) {
            const Piece* p = current_board_ptr->getPiece({b_r, b_c});
            if (!p || p->getColor() != input_state.current_player) {
                std::cout << "DEBUG: Invalid selection. Piece: " << (p ? "Yes" : "No") << ", Color: " << (p ? p->getColor() : -1) << std::endl;
                return;
            }
        }

        input_state.selected_r = b_r;
        input_state.selected_c = b_c;
        board_buttons[r][c]->color(FL_YELLOW);
        board_buttons[r][c]->redraw();
        std::cout << "DEBUG: Selected Source: " << b_r << "," << b_c << std::endl;
    } else {
        if (input_state.move_out_ptr) {
            input_state.move_out_ptr->from = {input_state.selected_r, input_state.selected_c};
            input_state.move_out_ptr->to = {b_r, b_c};
            input_state.move_out_ptr->player_color = input_state.current_player;
            std::cout << "DEBUG: Move constructed to: " << b_r << "," << b_c << std::endl;
        }
        
        int ui_sel_r = 7 - input_state.selected_r;
        int ui_sel_c = input_state.selected_c;
        board_buttons[ui_sel_r][ui_sel_c]->color(FL_GRAY);
        
        input_state.move_completed = true;
        input_state.waiting_for_move = false;
    }
}

void FltkIO::cb_board_click(Fl_Widget* w, void* data) {
    FltkIO* io = (FltkIO*)data;
    
    for(int r=0; r<8; ++r) {
        for(int c=0; c<8; ++c) {
            if (io->board_buttons[r][c] == w) {
                io->handle_board_click(r, c);
                return;
            }
        }
    }
}

void FltkIO::cb_end_game(Fl_Widget* w, void* data) {
    (void)w;
    FltkIO* io = (FltkIO*)data;
    io->handle_end_game();
}

void FltkIO::handle_end_game() {
    if (input_state.waiting_for_move) {
        if (input_state.move_out_ptr) {
            input_state.move_out_ptr->from = {-1, -1};
        }
        input_state.move_completed = true;
    }
}

void FltkIO::cb_quit(Fl_Widget* w, void* data) {
    (void)w;
    FltkIO* io = (FltkIO*)data;
    io->handle_quit();
}

void FltkIO::handle_quit() {
    exit(0);
}

void FltkIO::cb_new_game(Fl_Widget* w, void* data) {
    (void)w;
    FltkIO* io = (FltkIO*)data;
    io->handle_new_game();
}

void FltkIO::handle_new_game() {
    input_state.new_game_requested = true;
}