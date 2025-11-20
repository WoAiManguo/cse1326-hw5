#include "piece.hpp"


Piece::Piece(Color c, bool k) : color(c), isKing(k) {}

Color Piece::getColor() const { return color; }
bool Piece::getKing() const { return isKing; }

void Piece::makeKing() { isKing = true; }

char Piece::getSymbol() const {
    if (color == RED) { return (isKing) ? 'R' : 'r'; }
    else { return (isKing) ? 'B' : 'b'; }
}