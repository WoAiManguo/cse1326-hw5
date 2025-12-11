#pragma once

#include "position.hpp"
#include "piece.hpp"


struct Move {
    Position from;
    Position to;
    Color player_color;
    bool is_multijump;

    mutable char move_string[64];

    const char* to_char() const;
};