#pragma once

#include "board.hpp"
#include "move.hpp"
#include <vector>

class GameIO {
public:
    virtual void displayBoard(const Board& board, const char* status_msg, const char* error_msg) = 0;
    virtual bool getPlayerMove(Color player, Move* move_out) = 0;
    virtual void displayHistory(const std::vector<Move>& history) = 0;
    virtual ~GameIO() {}
};