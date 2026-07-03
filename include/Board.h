//
// Created by Mercury on 23/02/2026.
//

#ifndef CHESS_ENGINE_BOARD_H
#define CHESS_ENGINE_BOARD_H

#include <cstdint>
#include <unordered_map>
#include <vector>

#include "Move.h"
#include "../Enums/Piece.h"
#include "Zobrist.h"

struct UndoInfo {
    uint64_t enPassantSquare;
    int castlingRights; // 4-bitmask of castling rights, white castling, then black castling
                        // e.g. 1001 means right can short castle, and black can long castle
    Piece capturedPiece;
};

class Board {
    public:
        Board();
        void makeMove(const Move& move);
        void unmakeMove(const Move& move);

        [[nodiscard]] static uint64_t getLeastSignificantBit(uint64_t bitboard);
        [[nodiscard]] static uint64_t getMostSignificantBit(uint64_t bitboard);

        void setBoardSquare(uint64_t square, Piece piece);
        [[nodiscard]] Piece getPieceAtSquare(uint64_t square) const;

        void clearBoard();

        static uint64_t shift(uint64_t bitboard, int shiftAmount);

        [[nodiscard]] uint64_t getWhitePawns() const;
        void setWhitePawns(uint64_t bb);

        [[nodiscard]] uint64_t getWhiteKnights() const;
        void setWhiteKnights(uint64_t bb);

        [[nodiscard]] uint64_t getWhiteBishops() const;
        void setWhiteBishops(uint64_t bb);

        [[nodiscard]] uint64_t getWhiteRooks() const;
        void setWhiteRooks(uint64_t bb);

        [[nodiscard]] uint64_t getWhiteQueens() const;
        void setWhiteQueens(uint64_t bb);

        [[nodiscard]] uint64_t getWhiteKings() const;
        void setWhiteKings(uint64_t bb);

        [[nodiscard]] uint64_t getBlackPawns() const;
        void setBlackPawns(uint64_t bb);

        [[nodiscard]] uint64_t getBlackKnights() const;
        void setBlackKnights(uint64_t bb);

        [[nodiscard]] uint64_t getBlackBishops() const;
        void setBlackBishops(uint64_t bb);

        [[nodiscard]] uint64_t getBlackRooks() const;
        void setBlackRooks(uint64_t bb);

        [[nodiscard]] uint64_t getBlackQueens() const;
        void setBlackQueens(uint64_t bb);

        [[nodiscard]] uint64_t getBlackKings() const;
        void setBlackKings(uint64_t bb);

        [[nodiscard]] uint64_t getWhitePieces() const;
        [[nodiscard]] uint64_t getBlackPieces() const;
        [[nodiscard]] uint64_t getAllPieces() const;

        [[nodiscard]] std::array<uint64_t, 12> getBitboards();

        Color whoseTurn = Color::WHITE;
        // enPassantSquare is the bitboard representation of the square that the capturing pawn ends up on, not the square of the pawn that gets taken
        uint64_t enPassantSquare;
        int castlingRights;
        std::vector<UndoInfo> history;

        uint64_t zobristHash = 0;
        std::vector<uint64_t> zobristHistory;
    private:
        uint64_t bitboards[12]{};
        Piece pieces[12] = {
            Piece::WHITE_PAWN,
            Piece::WHITE_KNIGHT,
            Piece::WHITE_BISHOP,
            Piece::WHITE_ROOK,
            Piece::WHITE_QUEEN,
            Piece::WHITE_KING,
            Piece::BLACK_PAWN,
            Piece::BLACK_KNIGHT,
            Piece::BLACK_BISHOP,
            Piece::BLACK_ROOK,
            Piece::BLACK_QUEEN,
            Piece::BLACK_KING
        };
};


#endif //CHESS_ENGINE_BOARD_H