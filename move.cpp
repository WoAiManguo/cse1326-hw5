#include "move.hpp"
#include <cstdio>

const char* Move::to_char() const { 
    char from_str[3];
    char to_str[3];

    from.ind_to_str(from_str);
    to.ind_to_str(to_str);

    if (is_multijump) {
        snprintf(move_string, sizeof(move_string), "%s to %s (Multi-jump)", from_str, to_str);
    } else {
        snprintf(move_string, sizeof(move_string), "%s to %s", from_str, to_str);
    }

    return move_string;
}