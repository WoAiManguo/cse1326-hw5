#pragma once

#include "io.hpp"
#include <cstdio>

class ConsoleIO : public GameIO {
public:
    void displayBoard(const Board& board, const char* status_msg, const char* error_msg) override;
    bool getPlayerMove(Color player, Move* move_out) override;
    void displayHistory(const std::vector<Move>& history) override;
};