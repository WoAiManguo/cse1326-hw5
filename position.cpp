#include "position.hpp"
#include <cstring>

void Position::ind_to_str(char* out_str) const {
    if (out_str) {
        out_str[0] = (char)('A' + col);
        out_str[1] = (char)('1' + row);
        out_str[2] = '\0';
    }
}
    
Position Position::str_to_ind(const char* s) {
    Position pos = {-1, -1};
    if (s && strlen(s) == 2) {
        char col_char = s[0];
        char row_char = s[1];

        if (col_char >= 'A' && col_char <= 'H') {
            pos.col = col_char - 'A';
        }

        if (row_char >= '1' && row_char <= '8') {
            pos.row = row_char - '1';
        }
    }
    return pos;
}