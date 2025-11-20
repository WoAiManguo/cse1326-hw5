#pragma once

#include "piece.hpp"
#include "position.hpp"
#include "move.hpp"
#include <vector>

enum MoveError {
    NO_ERROR,
    ERROR_START_INVALID,
    ERROR_END_INVALID,
    ERROR_NO_PIECE,
    ERROR_WRONG_COLOR,
    ERROR_DEST_OCCUPIED,
    ERROR_DEST_WHITE_SQUARE,
    ERROR_NOT_DIAGONAL,
    ERROR_REGULAR_BACKWARD,
    ERROR_JUMP_MANDATORY,
    ERROR_JUMP_NO_CAPTURE,
    ERROR_JUMP_OWN_PIECE,
    ERROR_TOO_FAR
};

class Board {
private:
    Piece* grid[8][8];
    int red_pieces;
    int black_pieces;

    bool isKingRow(const Position& pos, Color color) const;
    MoveError validateJump(const Move& m, int dr, int dc) const;
    bool isJumpAvailable(Color player_color) const;
public:
    Board();
    ~Board();

    Board(const Board& other);

    const Piece* getPiece(const Position& pos) const;

    void initialize();

    int getRedCount() const { return red_pieces; }
    int getBlackCount() const { return black_pieces; }

    
    MoveError isLegalMove(const Move& m) const; 

    bool movePiece(const Move& m);


    const Piece* (*getGrid() const)[8];
};
