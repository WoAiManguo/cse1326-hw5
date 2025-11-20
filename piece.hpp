#pragma once

enum Color {
    RED,
    BLACK
};

class Piece {
private:
    Color color;
    bool isKing;
public:
    Piece(Color c, bool k = false);

    Color getColor() const;
    bool getKing() const;

    void makeKing();

    char getSymbol() const;
};