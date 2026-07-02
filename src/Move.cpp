//
// Created by Mercury on 23/02/2026.
//

#include "../include/Move.h"

#include <cstdint>

#include "../Enums/Piece.h"

/**
 * Unlike for the TUI, these squares refer to the number of bits needed to shift to get to that position.
 * Square 0 is the bottom left, 63 is the top right
 */
Move::Move(const uint64_t startSquare, const uint64_t endSquare, const Piece piece) {
    Move::startSquare = startSquare;
    Move::endSquare = endSquare;
    Move::piece = piece;
}

uint64_t Move::getStartSquare() const {
    return startSquare;
}

uint64_t Move::getEndSquare() const {
    return endSquare;
}

Piece Move::getPiece() const {
    return piece;
}

Color Move::getPieceColor() const {
    if (static_cast<int>(piece) <= 5 && static_cast<int>(piece) >= 0) {
        return Color::WHITE;
    }
    if (static_cast<int>(piece) > 5) {
        return Color::BLACK;
    }
    return Color::NONE;
}