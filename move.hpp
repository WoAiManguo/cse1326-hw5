#pragma once

#include "position.hpp"
#include "piece.hpp"


struct Move {
    Position from;
    Position to;
    Color player_color;

    mutable char move_string[16]; 

    const char* to_char() const; 
};