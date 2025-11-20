#pragma once

struct Position {
    int row;
    int col;

    void ind_to_str(char* out_str) const;
    
    static Position str_to_ind(const char* s);

    bool operator==(const Position& other) const { return row == other.row && col == other.col; }

    bool is_valid() const { return row >= 0 && row < 8 && col >= 0 && col < 8; }
};