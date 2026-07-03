//
// Created by Mercury on 24/02/2026.
//

#include "../include/DebugUtils.h"

#include <array>
#include <bitset>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "../include/BoardManager.h"

time_point<steady_clock> DebugUtils::timeBeforeMove;
int DebugUtils::numExploredPositions = 0;
int DebugUtils::numExploredQuiescencePositions = 0;
int DebugUtils::numTranspositionTableHits = 0;

void DebugUtils::printBitboard(uint64_t bitboard) {
    if (!debugMode) {
        return;
    }
    for (int rank = 7; rank >= 0; --rank) {
        std::cout << rank + 1 << "  ";  // Print rank number

        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;

            if (bitboard & (1ULL << square))
                std::cout << "1 ";
            else
                std::cout << ". ";
        }

        std::cout << "\n";
    }

    std::cout << "\n   a b c d e f g h\n\n";
}

void DebugUtils::printBoardState(const Board& board) {
    if (!debugMode) {
        return;
    }
    std::array<Piece, 64> pieces{};
    for (int i = 0; i < 64; i++) {
        pieces.at(i) = board.getPieceAtSquare(1ULL << i);
    }

    for (int rank = 7; rank >= 0; --rank) {
        if (rank == 7) {
            std::cout << "     ┏━━━━━┳━━━━━┳━━━━━┳━━━━━┳━━━━━┳━━━━━┳━━━━━┳━━━━━┓\n";
        }

        std::cout << "  " << rank + 1 << "  "; // Print rank number
        for (int file = 0; file < 8; ++file) {
            int index = rank * 8 + file;
            std::cout << "┃  " << pieceToChar(pieces[index]) << "  ";
        }
        std::cout << "┃\n";

        if (rank != 0) {
            std::cout << "     ┣━━━━━╋━━━━━╋━━━━━╋━━━━━╋━━━━━╋━━━━━╋━━━━━╋━━━━━┫\n";
        } else {
            std::cout << "     ┗━━━━━┻━━━━━┻━━━━━┻━━━━━┻━━━━━┻━━━━━┻━━━━━┻━━━━━┛\n";
            std::cout << "        A     B     C     D     E     F     G     H   \n";
        }
    }
    std::string enPassantSquare = squareToString(board.enPassantSquare);
    if (enPassantSquare == "Invalid Square") {
        enPassantSquare = "None";
    }
    std::cout << "  En-Passant Square: " << enPassantSquare << '\n';
    std::cout << "  Castling rights: " << std::bitset<4>(board.castlingRights) << "\n";
    std::cout << "  Halfmove Clock: TODO\n";
    std::cout << "  Fullmove Clock: TODO\n";
    std::cout << "  Zobrist Hash: " << board.zobristHash << "\n";
}

void DebugUtils::prettyPrintMoveList(const std::vector<Move>& moves) {
    if (!debugMode) {
        return;
    }
    for (const auto & move : moves) {
        prettyPrintMove(move);
    }
}

void DebugUtils::prettyPrintMove(const Move& move) {
    if (!debugMode) {
        return;
    }
    std::cout << moveToString(move) << '\n';
}

std::string DebugUtils::moveToString(const Move& move) {
    std::string returnString;

    switch (move.getPiece()) {
        case Piece::WHITE_PAWN:   returnString = "WhitePawn"; break;
        case Piece::WHITE_KNIGHT: returnString = "WhiteKnight"; break;
        case Piece::WHITE_BISHOP: returnString = "WhiteBishop"; break;
        case Piece::WHITE_ROOK:   returnString = "WhiteRook"; break;
        case Piece::WHITE_QUEEN:  returnString = "WhiteQueen"; break;
        case Piece::WHITE_KING:   returnString = "WhiteKing"; break;

        case Piece::BLACK_PAWN:   returnString = "BlackPawn"; break;
        case Piece::BLACK_KNIGHT: returnString = "BlackKnight"; break;
        case Piece::BLACK_BISHOP: returnString = "BlackBishop"; break;
        case Piece::BLACK_ROOK:   returnString = "BlackRook"; break;
        case Piece::BLACK_QUEEN:  returnString = "BlackQueen"; break;
        case Piece::BLACK_KING:   returnString = "BlackKing"; break;

        default:
            returnString = "UnknownPiece";
            break;
    }

    returnString += " from ";
    returnString += squareToString(move.getStartSquare());
    returnString += " to ";
    returnString += squareToString(move.getEndSquare());
    if (move.isCapture) {
        returnString += " CAPTURE";
    }
    if (move.enPassant) {
        returnString += " EN PASSANT";
    }
    if (move.isShortCastling) {
        returnString += " SHORT CASTLE";
    }
    if (move.isLongCastling) {
        returnString += " LONG CASTLE";
    }
    if (move.isPromotion) {
        returnString += " PROMOTION TO ";
        switch (move.promotionType) {
            case Piece::WHITE_KNIGHT:
            case Piece::BLACK_KNIGHT: returnString += " KNIGHT"; break;
            case Piece::WHITE_BISHOP:
            case Piece::BLACK_BISHOP: returnString += " BISHOP"; break;
            case Piece::WHITE_ROOK:
            case Piece::BLACK_ROOK: returnString += " ROOK"; break;
            case Piece::WHITE_QUEEN:
            case Piece::BLACK_QUEEN: returnString += " QUEEN"; break;
            default: returnString += " UnknownPiece";
        }
    }
    return returnString;
}

std::string DebugUtils::squareToString(const uint64_t square) {
    if (square == 0) {
        return "Invalid Square";
    }
    int lsbIndex = 0;
    bool alreadyFoundBit = false;
    for (int i = 0; i < 64; i++) {
        if ((square & 1ULL << i) > 0) {
            if (alreadyFoundBit) {
                return "Invalid Square";
            }
            lsbIndex = i;
            alreadyFoundBit = true;
        }
    }
    char file = static_cast<char>('a' + lsbIndex % 8);
    int rank = lsbIndex / 8 + 1;
    std::string returnString;
    returnString.push_back(file);
    returnString += std::to_string(rank);
    return returnString;

}

char DebugUtils::pieceToChar(Piece piece) {
    std::unordered_map<Piece, char> pieceToCharMap {
            {Piece::WHITE_PAWN, 'P'},
            {Piece::WHITE_KNIGHT, 'N'},
            {Piece::WHITE_BISHOP, 'B'},
            {Piece::WHITE_ROOK, 'R'},
            {Piece::WHITE_QUEEN, 'Q'},
            {Piece::WHITE_KING, 'K'},
            {Piece::BLACK_PAWN, 'p'},
            {Piece::BLACK_KNIGHT, 'n'},
            {Piece::BLACK_BISHOP, 'b'},
            {Piece::BLACK_ROOK, 'r'},
            {Piece::BLACK_QUEEN, 'q'},
            {Piece::BLACK_KING, 'k'},
            {Piece::EMPTY, ' '}
    };
    return pieceToCharMap.at(piece);
}

void DebugUtils::debugPrint(std::string s) {
    if (debugMode) {
        std::cout << s << std::endl;
    }
}

void DebugUtils::startMoveTimer() {
    timeBeforeMove = std::chrono::steady_clock::now();
}

void DebugUtils::printMoveTime() {
    if (!debugMode) return;
    const auto timeAfterMove = steady_clock::now();
    const auto timeTaken = std::chrono::duration_cast<std::chrono::milliseconds>(
        timeAfterMove - timeBeforeMove
    );
    std::cout << timeTaken.count()
        << "ms taken for search with depth "
        << BoardManager::depth << '\n';
}

void DebugUtils::printNumExploredPositions() {
    if (!debugMode) return;
    std::cout << "Explored " << numExploredPositions << " positions." << std::endl;
    std::cout << "Explored " << numExploredQuiescencePositions << " quiescence positions." << std::endl;
    std::cout << "Had " << numTranspositionTableHits << " transposition table hits." << std::endl;
    numExploredPositions = 0;
    numExploredQuiescencePositions = 0;
    numTranspositionTableHits = 0;
}






