//
// Created by Mercury on 23/02/2026.
//

#ifndef CHESS_ENGINE_PIECEENUM_H
#define CHESS_ENGINE_PIECEENUM_H


enum class Piece {
    WHITE_PAWN = 0,
    WHITE_KNIGHT = 1,
    WHITE_BISHOP = 2,
    WHITE_ROOK = 3,
    WHITE_QUEEN = 4,
    WHITE_KING = 5,
    BLACK_PAWN = 6,
    BLACK_KNIGHT = 7,
    BLACK_BISHOP = 8,
    BLACK_ROOK = 9,
    BLACK_QUEEN = 10,
    BLACK_KING = 11,
    EMPTY = -1
};

inline bool isPawn(const Piece piece) {
    if (piece == Piece::WHITE_PAWN || piece == Piece::BLACK_PAWN) {
        return true;
    }
    return false;
}

inline bool isKnight(const Piece piece) {
    if (piece == Piece::WHITE_KNIGHT || piece == Piece::BLACK_KNIGHT) {
        return true;
    }
    return false;
}

inline bool isBishop(const Piece piece) {
    if (piece == Piece::WHITE_BISHOP || piece == Piece::BLACK_BISHOP) {
        return true;
    }
    return false;
}

inline bool isRook(const Piece piece) {
    if (piece == Piece::WHITE_ROOK || piece == Piece::BLACK_ROOK) {
        return true;
    }
    return false;
}

inline bool isQueen(const Piece piece) {
    if (piece == Piece::WHITE_QUEEN || piece == Piece::BLACK_QUEEN) {
        return true;
    }
    return false;
}

inline bool isKing(const Piece piece) {
    if (piece == Piece::WHITE_KING || piece == Piece::BLACK_KING) {
        return true;
    }
    return false;
}

#endif //CHESS_ENGINE_PIECEENUM_H