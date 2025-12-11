#pragma once

#include "io.hpp"
#include <cstdio>

#include <Fl/Fl_Window.H>
#include <Fl/Fl_Button.H>
#include <Fl/Fl_Browser.H>
#include <Fl/Fl_Group.H>
#include <Fl/Fl_Box.H>
#include <string>
#include <vector>

class FltkIO : public GameIO {
public:
    FltkIO();
    ~FltkIO();
    void displayBoard(const Board& board, const char* status_msg, const char* error_msg) override;
    bool getPlayerMove(Color player, Move* move_out) override;
    void displayHistory(const std::vector<Move>& history) override;
    void waitForContinuation() override;
    void processEvents() override;

private:
    Fl_Window* window;
    Fl_Button* board_buttons[8][8];
    Fl_Browser* history_view;
    Fl_Button* btn_new_game;
    Fl_Button* btn_end_game;
    Fl_Button* btn_quit;
    Fl_Box* status_box;
    const Board* current_board_ptr;

    struct {
        bool waiting_for_move;
        bool move_completed;
        int selected_r, selected_c;
        Move* move_out_ptr;
        Color current_player;
        bool new_game_requested;
    } input_state;

    static void cb_board_click(Fl_Widget* w, void* data);
    void handle_board_click(int r, int c);
    
    static void cb_end_game(Fl_Widget* w, void* data);
    void handle_end_game();

    static void cb_quit(Fl_Widget* w, void* data);
    void handle_quit();

    static void cb_new_game(Fl_Widget* w, void* data);
    void handle_new_game();
};