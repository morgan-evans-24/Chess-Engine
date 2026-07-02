//
// Created by Mercury on 23/02/2026.
//

#include "../include/Board.h"

#include <array>
#include <atomic>

#include "../include/Zobrist.h"
#include "../include/DebugUtils.h"
#include "../Enums/Piece.h"

Board::Board() {
    enPassantSquare = 0;
    castlingRights = 0b1111;
    history = std::vector<UndoInfo>{};
};

// TODO This needs to update castling permissions on king and rook moves, not just castling. This is necessary for Zobrist hashing
// TODO Then, same needs to be done for en-passant.
void Board::makeMove(const Move& move) {
    const uint64_t endSquareBB = move.getEndSquare();
    const uint64_t startSquareBB = move.getStartSquare();

    Piece piece = move.getPiece();
    Piece targetPiece = getPieceAtSquare(endSquareBB);

    // Save info about the move for rewinding
    // This is the info about the board BEFORE making the move
    UndoInfo undo{};
    undo.enPassantSquare = enPassantSquare;
    undo.castlingRights = castlingRights;
    undo.capturedPiece = targetPiece;
    history.push_back(undo);

    // Undo enPassant in Zobrist
    if (enPassantSquare) {
        zobristHash ^= Zobrist::enPassantKeys[__builtin_ctzll(enPassantSquare)];
    }
    // Undo old castle rights in Zobrist
    zobristHash ^= Zobrist::castleKeys[castlingRights];

    if (isPawn(piece)) {
        // Move is double pawn advance
        int shiftAmount = whoseTurn == Color::WHITE ? 8 : -8;
        if (endSquareBB == shift(startSquareBB, 2*shiftAmount)) {
            enPassantSquare = shift(startSquareBB, shiftAmount);
        } else {
            enPassantSquare = 0;
        }
    } else {
        enPassantSquare = 0;
    }


    if (isKing(piece)) {
        if (whoseTurn == Color::WHITE) {
            castlingRights &= 0b0011;
        } else {
            castlingRights &= 0b1100;
        }
    }

    constexpr uint64_t A1 = 1ULL << 0;
    constexpr uint64_t H1 = 1ULL << 7;
    constexpr uint64_t A8 = 1ULL << 56;
    constexpr uint64_t H8 = 1ULL << 63;
    if (piece == Piece::WHITE_ROOK) {
        if (startSquareBB & A1)
            castlingRights &= 0b0111;
        else if (startSquareBB & H1)
            castlingRights &= 0b1011;
    }

    if (piece == Piece::BLACK_ROOK) {
        if (startSquareBB & A8)
            castlingRights &= 0b1101;
        else if (startSquareBB & H8)
            castlingRights &= 0b1110;
    }



    // If move is en-passant, flip the enemy pawns square
    if (move.enPassant) {
        Piece enPassantedPiece;
        if (piece == Piece::WHITE_PAWN) {
            enPassantedPiece = Piece::BLACK_PAWN;
        } else {
            enPassantedPiece = Piece::WHITE_PAWN;
        }
        const uint64_t enPassantedSquare = whoseTurn == Color::WHITE ? shift(endSquareBB, -8) : shift(endSquareBB, 8);
        uint64_t& enemyPawnBb = whoseTurn == Color::WHITE ? bitboards[6] : bitboards[0];
        enemyPawnBb ^= enPassantedSquare;
        zobristHash ^= Zobrist::pieceKeys[static_cast<int>(enPassantedPiece)][__builtin_ctzll(enPassantedSquare)];
    }

    if (move.isShortCastling) {
        uint64_t shortRookStartSquare = whoseTurn == Color::WHITE ? 0x80ULL : 0x8000000000000000ULL;
        uint64_t shortRookEndSquare = whoseTurn == Color::WHITE ? 0x20ULL : 0x2000000000000000ULL;
        uint64_t& rookBb = whoseTurn == Color::WHITE ? bitboards[3] : bitboards[9];
        rookBb ^= shortRookStartSquare;
        rookBb ^= shortRookEndSquare;
        if (whoseTurn == Color::WHITE) {
            castlingRights &= 0b0011;
            zobristHash ^= Zobrist::pieceKeys[static_cast<int>(Piece::WHITE_ROOK)][__builtin_ctzll(shortRookStartSquare)];
            zobristHash ^= Zobrist::pieceKeys[static_cast<int>(Piece::WHITE_ROOK)][__builtin_ctzll(shortRookEndSquare)];
        } else {
            castlingRights &= 0b1100;
            zobristHash ^= Zobrist::pieceKeys[static_cast<int>(Piece::BLACK_ROOK)][__builtin_ctzll(shortRookStartSquare)];
            zobristHash ^= Zobrist::pieceKeys[static_cast<int>(Piece::BLACK_ROOK)][__builtin_ctzll(shortRookEndSquare)];
        }
    } else if (move.isLongCastling) {
        uint64_t longRookStartSquare = whoseTurn == Color::WHITE ? 0x01ULL : 0x0100000000000000ULL;
        uint64_t longRookEndSquare = whoseTurn == Color::WHITE ? 0x08ULL : 0x0800000000000000ULL;
        uint64_t& rookBb = whoseTurn == Color::WHITE ? bitboards[3] : bitboards[9];
        rookBb ^= longRookStartSquare;
        rookBb ^= longRookEndSquare;
        if (whoseTurn == Color::WHITE) {
            castlingRights &= 0b0011;
            zobristHash ^= Zobrist::pieceKeys[static_cast<int>(Piece::WHITE_ROOK)][__builtin_ctzll(longRookStartSquare)];
            zobristHash ^= Zobrist::pieceKeys[static_cast<int>(Piece::WHITE_ROOK)][__builtin_ctzll(longRookEndSquare)];
        } else {
            castlingRights &= 0b1100;
            zobristHash ^= Zobrist::pieceKeys[static_cast<int>(Piece::BLACK_ROOK)][__builtin_ctzll(longRookStartSquare)];
            zobristHash ^= Zobrist::pieceKeys[static_cast<int>(Piece::BLACK_ROOK)][__builtin_ctzll(longRookEndSquare)];
        }
    }



    // change where the attacking piece is
    uint64_t* pieceBitboard = &bitboards[static_cast<int>(piece)];
    zobristHash ^= Zobrist::pieceKeys[static_cast<int>(piece)][__builtin_ctzll(startSquareBB)];
    *pieceBitboard ^= startSquareBB;

    if (move.isPromotion) {
        pieceBitboard = &bitboards[static_cast<int>(move.promotionType)];
        zobristHash ^= Zobrist::pieceKeys[static_cast<int>(move.promotionType)][__builtin_ctzll(endSquareBB)];
    } else {
        zobristHash ^= Zobrist::pieceKeys[static_cast<int>(piece)][__builtin_ctzll(endSquareBB)];
    }
    *pieceBitboard ^= endSquareBB;

    // If the target piece isn't empty, update it's bitboard
    if (targetPiece != Piece::EMPTY) {
        if (targetPiece == Piece::WHITE_ROOK) {
            if (endSquareBB & A1) {
                castlingRights &= 0b0111;
            }
            else if (endSquareBB & H1) {
                castlingRights &= 0b1011;
            }
        }

        if (targetPiece == Piece::BLACK_ROOK) {
            if (endSquareBB & A8) {
                castlingRights &= 0b1101;
            }
            else if (endSquareBB & H8) {
                castlingRights &= 0b1110;
            }
        }
        uint64_t& targetBitboard = bitboards[static_cast<int>(targetPiece)];
        zobristHash ^= Zobrist::pieceKeys[static_cast<int>(targetPiece)][__builtin_ctzll(endSquareBB)];
        targetBitboard ^= endSquareBB;
    }
    // Flip turn clock
    whoseTurn = whoseTurn == Color::WHITE ? Color::BLACK : Color::WHITE;

    // Apply new castling rights through Zobrist
    zobristHash ^= Zobrist::castleKeys[castlingRights];

    // Apply new en-passant rights through Zobrist (if there are any)
    if (enPassantSquare) {
        zobristHash ^= Zobrist::enPassantKeys[__builtin_ctzll(enPassantSquare)];
    }

    // Flip turn in Zobrist
    zobristHash ^= Zobrist::blackToMoveKey;


}

void Board::unmakeMove(const Move& move) {
    const uint64_t endSquareBB = move.getEndSquare();
    const uint64_t startSquareBB = move.getStartSquare();
    Piece piece = move.getPiece();

    // Undo enPassant zobrist
    if (enPassantSquare) {
        zobristHash ^= Zobrist::enPassantKeys[__builtin_ctzll(enPassantSquare)];
    }

    // Undo castling zobrist
    zobristHash ^= Zobrist::castleKeys[castlingRights];


    // pop the history from the stack, restore board position
    UndoInfo undo = history.back();
    enPassantSquare = undo.enPassantSquare;
    castlingRights = undo.castlingRights;
    Piece targetPiece = undo.capturedPiece;
    history.pop_back();

    // Apply old en-passant zobrist
    if (enPassantSquare) {
        zobristHash ^= Zobrist::enPassantKeys[__builtin_ctzll(enPassantSquare)];
    }

    // Apply old castling zobrist
    zobristHash ^= Zobrist::castleKeys[castlingRights];

    // Undo move clock zobrist
    zobristHash ^= Zobrist::blackToMoveKey;

    // If move was en-passant, reverse it by flipping the enemy pawns bitboard back
    if (move.enPassant) {
        Piece enemyPawnPiece = whoseTurn == Color::BLACK ? Piece::BLACK_PAWN : Piece::WHITE_PAWN;
        uint64_t enPassantedSquare = whoseTurn == Color::BLACK ? shift(endSquareBB, -8) : shift(endSquareBB, 8);
        uint64_t& enemyPawnBb = whoseTurn == Color::BLACK ? bitboards[6] : bitboards[0];
        enemyPawnBb ^= enPassantedSquare;
        zobristHash ^= Zobrist::pieceKeys[static_cast<int>(enemyPawnPiece)][__builtin_ctzll(enPassantedSquare)];
    }

    // If move was shortcastling, reset change
    if (move.isShortCastling) {
        uint64_t shortRookStartSquare = whoseTurn == Color::BLACK ? 0x20ULL : 0x2000000000000000ULL;
        uint64_t shortRookEndSquare = whoseTurn == Color::BLACK ? 0x80ULL : 0x8000000000000000ULL;
        uint64_t& rookBb = whoseTurn == Color::BLACK ? bitboards[3] : bitboards[9];
        Piece rook = whoseTurn == Color::BLACK ? Piece::WHITE_ROOK : Piece::BLACK_ROOK;
        rookBb ^= shortRookStartSquare;
        rookBb ^= shortRookEndSquare;

        zobristHash ^= Zobrist::pieceKeys[static_cast<int>(rook)][__builtin_ctzll(shortRookStartSquare)];
        zobristHash ^= Zobrist::pieceKeys[static_cast<int>(rook)][__builtin_ctzll(shortRookEndSquare)];
    }

    // If move was longcastling, reset change
    if (move.isLongCastling) {
        uint64_t longRookStartSquare = whoseTurn == Color::BLACK ? 0x08ULL : 0x0800000000000000ULL;
        uint64_t longRookEndSquare = whoseTurn == Color::BLACK ? 0x01ULL : 0x0100000000000000ULL;
        uint64_t& rookBb = whoseTurn == Color::BLACK ? bitboards[3] : bitboards[9];
        Piece rook = whoseTurn == Color::BLACK ? Piece::WHITE_ROOK : Piece::BLACK_ROOK;
        rookBb ^= longRookStartSquare;
        rookBb ^= longRookEndSquare;

        zobristHash ^= Zobrist::pieceKeys[static_cast<int>(rook)][__builtin_ctzll(longRookStartSquare)];
        zobristHash ^= Zobrist::pieceKeys[static_cast<int>(rook)][__builtin_ctzll(longRookEndSquare)];
    }

    // Change where the attacking piece is
    uint64_t* pieceBitboard = &bitboards[static_cast<int>(piece)];
    zobristHash ^= Zobrist::pieceKeys[static_cast<int>(piece)][__builtin_ctzll(startSquareBB)];
    *pieceBitboard ^= startSquareBB;
    if (move.isPromotion) {
        pieceBitboard = &bitboards[static_cast<int>(move.promotionType)];
        zobristHash ^= Zobrist::pieceKeys[static_cast<int>(move.promotionType)][__builtin_ctzll(endSquareBB)];
    } else {
        zobristHash ^= Zobrist::pieceKeys[static_cast<int>(piece)][__builtin_ctzll(endSquareBB)];
    }
    *pieceBitboard ^= endSquareBB;

    // If there was a target piece, return it to its position
    if (targetPiece != Piece::EMPTY) {
        uint64_t& targetBitboard = bitboards[static_cast<int>(targetPiece)];
        targetBitboard ^= endSquareBB;
        zobristHash ^= Zobrist::pieceKeys[static_cast<int>(targetPiece)][__builtin_ctzll(endSquareBB)];
    }

    // Flip turn clock
    whoseTurn = whoseTurn == Color::WHITE ? Color::BLACK : Color::WHITE;

}

uint64_t Board::getLeastSignificantBit(uint64_t bitboard) {
    // AND the board with its two's complement version
    return bitboard & -bitboard;
}

uint64_t Board::getMostSignificantBit(uint64_t bitboard) {
    bitboard |= bitboard >> 32;
    bitboard |= bitboard >> 16;
    bitboard |= bitboard >> 8;
    bitboard |= bitboard >> 4;
    bitboard |= bitboard >> 2;
    bitboard |= bitboard >> 1;
    return (bitboard >> 1) + 1;
}

uint64_t Board::shift(uint64_t bitboard, int shiftAmount) {
    if (shiftAmount > 0) {
        return bitboard << shiftAmount;
    }
    if (shiftAmount < 0) {
        return bitboard >> -shiftAmount;
    }
    return bitboard;
}

// Square is the 0-63 square number here
void Board::setBoardSquare(uint64_t square, Piece piece) {
    uint64_t mask = ~(1ULL << square);
    for (auto& bb : bitboards) {
        bb &= mask;
    }

    bitboards[static_cast<int>(piece)] |= 1ULL << square;
}

// This function takes the bitboard representation of a square, not its integer.
Piece Board::getPieceAtSquare(uint64_t square) const {
    for (int i = 0; i < sizeof(bitboards) / sizeof(bitboards[0]); i++) {
        if ((square & bitboards[i]) > 0) {
            return pieces[i];
        }
    }
    return Piece::EMPTY;
}

void Board::clearBoard() {
    for (auto& bb : bitboards) {
        bb = 0;
    }
}


uint64_t Board::getWhitePawns() const {
    return bitboards[0];
}
void Board::setWhitePawns(uint64_t bb) {
    bitboards[0] = bb;
}

uint64_t Board::getWhiteKnights() const {
    return bitboards[1];
}
void Board::setWhiteKnights(uint64_t bb) {
    bitboards[1] = bb;
}

uint64_t Board::getWhiteBishops() const {
    return bitboards[2];
}
void Board::setWhiteBishops(uint64_t bb) {
    bitboards[2] = bb;
}

uint64_t Board::getWhiteRooks() const {
    return bitboards[3];
}
void Board::setWhiteRooks(uint64_t bb) {
    bitboards[3] = bb;
}

uint64_t Board::getWhiteQueens() const {
    return bitboards[4];
}
void Board::setWhiteQueens(uint64_t bb) {
    bitboards[4] = bb;
}

uint64_t Board::getWhiteKings() const {
    return bitboards[5];
}
void Board::setWhiteKings(uint64_t bb) {
    bitboards[5] = bb;
}

uint64_t Board::getBlackPawns() const {
    return bitboards[6];
}
void Board::setBlackPawns(uint64_t bb) {
    bitboards[6] = bb;
}

uint64_t Board::getBlackKnights() const {
    return bitboards[7];
}
void Board::setBlackKnights(uint64_t bb) {
    bitboards[7] = bb;
}

uint64_t Board::getBlackBishops() const {
    return bitboards[8];
}
void Board::setBlackBishops(uint64_t bb) {
    bitboards[8] = bb;
}

uint64_t Board::getBlackRooks() const {
    return bitboards[9];
}
void Board::setBlackRooks(uint64_t bb) {
    bitboards[9] = bb;
}

uint64_t Board::getBlackQueens() const {
    return bitboards[10];
}
void Board::setBlackQueens(uint64_t bb) {
    bitboards[10] = bb;
}

uint64_t Board::getBlackKings() const {
    return bitboards[11];
}
void Board::setBlackKings(uint64_t bb) {
    bitboards[11] = bb;
}

uint64_t Board::getAllPieces() const {
    return getWhitePieces() | getBlackPieces();
}
uint64_t Board::getWhitePieces() const {
    return getWhitePawns() | getWhiteKnights() | getWhiteBishops() | getWhiteRooks() | getWhiteQueens() | getWhiteKings();
}
uint64_t Board::getBlackPieces() const {
    return getBlackPawns() | getBlackKnights() | getBlackBishops() | getBlackRooks() | getBlackQueens() | getBlackKings();
}

std::array<uint64_t, 12> Board::getBitboards() {
    std::array<uint64_t, 12> copy{};
    std::copy(std::begin(bitboards), std::end(bitboards), copy.begin());
    return copy;
}


