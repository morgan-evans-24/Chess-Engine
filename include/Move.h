//
// Created by Mercury on 23/02/2026.
//

#ifndef CHESS_ENGINE_MOVE_H
#define CHESS_ENGINE_MOVE_H
#include <cstdint>

#include "../Enums/Color.h"
#include "../Enums/Piece.h"


enum class Piece;

class Move {
    public:
        Move(uint64_t startSquare, uint64_t endSquare, Piece piece);
        [[nodiscard]] Piece getPiece() const;
        [[nodiscard]] Color getPieceColor() const;
        [[nodiscard]] uint64_t getStartSquare() const;
        [[nodiscard]] uint64_t getEndSquare() const;

        bool operator==(const Move& other) const {
            return (piece == other.piece &&
                startSquare == other.startSquare &&
                endSquare == other.endSquare);
        }

        bool operator!=(const Move& other) const {
            return !(*this == other);
        }

        bool isEnPassantable = false;
        bool enPassant = false;
        bool isShortCastling = false;
        bool isLongCastling = false;
        bool isPromotion = false;
        bool isCapture = false;
        int moveScore = 0;
        Piece promotionType = Piece::EMPTY;
    private:
        Piece piece;
        uint64_t startSquare;
        uint64_t endSquare;

};


#endif //CHESS_ENGINE_MOVE_H