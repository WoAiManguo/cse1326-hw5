#pragma once

#include "board.hpp"
#include "io.hpp"
#include <vector>

class Game {
private:
    Board board;
    GameIO* io;
    std::vector<Move> move_history;
    Color current_player;
    bool game_over;
    char error_message[100];

    void initializeGame();
    bool handlePlayerTurn();
    bool handleMachineTurn();
    
    Color determineWinner() const;
    bool isGameOver() const;

public:
    Game(GameIO* game_io);
    
    void run();
};