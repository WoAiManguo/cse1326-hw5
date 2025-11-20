#include "board.hpp"
#include <cstdlib>
#include <cstdio>
#include <cstring>

bool Board::isKingRow(const Position& pos, Color color) const {
    if (color == RED) { return pos.row == 0; }
    else { return pos.row == 7; }
}

Board::Board() : red_pieces(0), black_pieces(0) {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            grid[r][c] = NULL;
        }
    }
}

Board::~Board() {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            delete grid[r][c];
            grid[r][c] = NULL;
        }
    }
}

Board::Board(const Board& other) : red_pieces(other.red_pieces), black_pieces(other.black_pieces) {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if (other.grid[r][c]) {
                grid[r][c] = new Piece(other.grid[r][c]->getColor());
                if (other.grid[r][c]->getKing()) {
                    grid[r][c]->makeKing();
                }
            } else { grid[r][c] = NULL; }
        }
    }
}

void Board::initialize() {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            delete grid[r][c];
            grid [r][c] = NULL;
        }
    }

    red_pieces = 0;
    black_pieces = 0;

    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 8; ++c) {
            if ((r+c) % 2 != 0) {
                grid[r][c] = new Piece(BLACK);
                black_pieces++;
            }
        }
    }

    for (int r = 5; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if ((r+c) % 2 != 0) {
                grid[r][c] = new Piece(RED);
                red_pieces++;
            }
        }
    }
}

const Piece* Board::getPiece(const Position& pos) const {
    if (pos.row >= 0 && pos.row < 8 && pos.col >= 0 && pos.col < 8) { return grid[pos.row][pos.col]; }
    return NULL;
}

bool Board::isJumpAvailable(Color player_color) const {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            const Piece* piece = grid[r][c];
            if (piece != NULL && piece->getColor() == player_color) {
                
                for (int dr_sign = -1; dr_sign <= 1; dr_sign += 2) {
                    for (int dc_sign = -1; dc_sign <= 1; dc_sign += 2) {
                        int dr = dr_sign * 2;
                        int dc = dc_sign * 2;
                        
                        Position end = {r + dr, c + dc};

                        if (end.is_valid() && getPiece(end) == NULL) {

                            if (!piece->getKing()) {
                                if (piece->getColor() == BLACK && dr < 0) continue;
                                if (piece->getColor() == RED && dr > 0) continue;
                            }

                            Position captured_pos = {r + (dr / 2), c + (dc / 2)};
                            const Piece* captured_piece = getPiece(captured_pos);

                            if (captured_piece != NULL && captured_piece->getColor() != player_color) {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}

MoveError Board::validateJump(const Move& m, int dr, int dc) const {
    const Piece* moveable_piece = getPiece(m.from); 
    
    if (!moveable_piece->getKing()) {
        if (moveable_piece->getColor() == BLACK) {
            if (dr < 0) return ERROR_REGULAR_BACKWARD; 
        } else {
            if (dr > 0) return ERROR_REGULAR_BACKWARD;
        }
    }

    Position captured_pos = {
        m.from.row + (dr / 2),
        m.from.col + (dc / 2)
    };
    
    const Piece* captured_piece = getPiece(captured_pos);
    
    if (captured_piece == NULL) {
        return ERROR_JUMP_NO_CAPTURE;
    }
    
    if (captured_piece->getColor() == m.player_color) {
        return ERROR_JUMP_OWN_PIECE;
    }

    return NO_ERROR; 
}

MoveError Board::isLegalMove(const Move& m) const {
    if (!m.from.is_valid()) return ERROR_START_INVALID;
    if (!m.to.is_valid()) return ERROR_END_INVALID;

    const Piece* moveable_piece = getPiece(m.from);

    if (moveable_piece == NULL) {
        return ERROR_NO_PIECE;
    }

    if (moveable_piece->getColor() != m.player_color) {
        return ERROR_WRONG_COLOR;
    }
    
    if (getPiece(m.to) != NULL) {
        return ERROR_DEST_OCCUPIED;
    }
    
    if ((m.to.row + m.to.col) % 2 == 0) {
        return ERROR_DEST_WHITE_SQUARE;
    }

    int dr = m.to.row - m.from.row; 
    int dc = m.to.col - m.from.col; 
    int abs_dr = abs(dr);
    
    if (abs(dc) != abs_dr || abs_dr == 0 || abs_dr > 2) {
        return (abs_dr > 2) ? ERROR_TOO_FAR : ERROR_NOT_DIAGONAL;
    }
    
    if (abs_dr == 1) {
        if (isJumpAvailable(m.player_color)) return ERROR_JUMP_MANDATORY; 
        
        if (!moveable_piece->getKing()) {
            if (moveable_piece->getColor() == BLACK) { 
                if (dr < 0) return ERROR_REGULAR_BACKWARD;
            } else { 
                if (dr > 0) return ERROR_REGULAR_BACKWARD;
            }
        }
        
        return NO_ERROR; 

    } else {
        return validateJump(m, dr, dc);
    }
}

bool Board::movePiece(const Move& m) {
    Piece* piece = grid[m.from.row][m.from.col];

    if (!piece || grid[m.to.row][m.to.col]) {
        return false;
    }

    grid[m.to.row][m.to.col] = piece;
    grid[m.from.row][m.from.col] = NULL;

    if (!piece->getKing() && isKingRow(m.to, piece->getColor())) { piece->makeKing(); }

    int dr = m.to.row - m.from.row;
    int dc = m.to.col - m.from.col;

    if (abs(dr) == 2 && abs(dc) == 2) {
        int captured_r = m.from.row + (dr / 2);
        int captured_c = m.from.col + (dc / 2);

        Position captured_pos = {captured_r, captured_c};
        Piece* captured_piece = grid[captured_pos.row][captured_pos.col];

        if (captured_piece) {
            if (captured_piece->getColor() == RED) {
                red_pieces--;
            } else {
                black_pieces--;
            }
            delete captured_piece;
            grid[captured_pos.row][captured_pos.col] = NULL;
        }
    }

    return true;
}

const Piece* (*Board::getGrid() const)[8] { return (const Piece* (*)[8])grid; }
