//
// Created by Mercury on 23/02/2026.
//

#include "../include/MoveGenerator.h"

#include <cassert>
#include <climits>
#include <iostream>
#include <ostream>
#include <ranges>
#include <vector>

#include "../include/Board.h"
#include "../Enums/Piece.h"
#include "../include/DebugUtils.h"
#include "../include/rook_table.h"
#include "../include/bishop_table.h"
#include "../include/RuleChecker.h"

using std::vector;
using std::pair;
using std::to_string;

auto shift = Board::shift;

MoveGenerator::MoveGenerator() {
    evaluator = Evaluator();
    std::srand(std::time(nullptr));
}

Move MoveGenerator::getBestMove(Board& board, const int depth) {
    vector<Move> moves;
    generateLegalMoves(moves, board, false);
    auto bestMove= Move(0,0,Piece::WHITE_PAWN);

    // Best guarenteed move for me so far
    int alpha = -INF;
    // Best guarenteed move for opponent so far
    int beta = INF;

    bool moveFound = false;
    int maxScore = -INF;
    for (const Move& move : moves) {
        uint64_t oldZobrist = board.zobristHash;
        board.makeMove(move);
        uint64_t newZobrist = board.zobristHash;
        int score;

        score = -negaMax(board, depth - 1, -beta, -alpha);
        assert(board.zobristHash == newZobrist);

        DebugUtils::numExploredPositions++;
        // For a depth of 3
        // generate moves once in here
        // flip result of negaMax
        board.unmakeMove(move);
        assert(board.zobristHash == oldZobrist);

        // DebugUtils::prettyPrintMove(move);

        if (score > alpha) {
            moveFound = true;
            alpha = score;
            bestMove = move;
        }
    }

    if (!moveFound) {
        return moves.front();
    }

    // DebugUtils::prettyPrintMove(bestMove);
    return bestMove;
}

int MoveGenerator::negaMax(Board& board, int depth, int alpha, int beta) {
    if (depth == 0) {
        return quiescence(board, alpha, beta, 5);
    }

    vector<Move> moves;
    generateLegalMoves(moves, board, false);

    if (moves.empty()) {
        // This is checkmate or stalemate
        if (RuleChecker::isColorInCheck(board, board.whoseTurn)) {
            return -(MATE_SCORE + depth);
        }
        return 0;
    }

    int bestScore = -INF;
    for (const Move& move : moves) {
        uint64_t oldZobrist = board.zobristHash;
        board.makeMove(move);
        uint64_t newZobrist = board.zobristHash;

        int score;

        score = -negaMax(board, depth - 1, -beta, -alpha);
        assert(board.zobristHash == newZobrist);

        DebugUtils::numExploredPositions++;
        board.unmakeMove(move);
        assert(board.zobristHash == oldZobrist);

        if (score >= beta) {
            return beta;
        }

        if (score > alpha) {
            alpha = score;
        }
    }
    return alpha;
}

int MoveGenerator::quiescence(Board& board, int alpha, int beta, int depth) {
    int currentScore = evaluator.evaluateBoard(board);

    if (depth == 0) {
        return currentScore;
    }

    if (currentScore >= beta) {
        return beta;
    }

    if (currentScore > alpha) {
        alpha = currentScore;
    }

    std::vector<Move> moves;
    generateLegalMoves(moves, board, true);

    for (const Move& move : moves) {
        uint64_t oldZobrist = board.zobristHash;
        board.makeMove(move);
        DebugUtils::numExploredQuiescencePositions++;
        int score = -quiescence(board, -beta, -alpha, depth - 1);
        board.unmakeMove(move);

        assert(board.zobristHash == oldZobrist);

        if (score >= beta) {
            return beta;
        }

        if (score > alpha) {
            alpha = score;
        }
    }
    return alpha;
}


/**
 *
 * @param board A reference to the current board object
 * @param bitboardPosition The position of the square you are testing in bitboard notation
 * @param color The color that is being attacked. I.e. passing Color::WHITE tests if black pieces are attacking the square
 * @return
 */
bool MoveGenerator::isSquareAttacked(Board& board, const uint64_t bitboardPosition, const Color color) {
    if (bitboardPosition == 0) {
        return true;
    }
    uint64_t superPieceAttacks = 0;
    uint64_t enemyBb = color == Color::WHITE ? board.getBlackPawns() : board.getWhitePawns();
    superPieceAttacks = getPawnAttacksForSquare(board, bitboardPosition, color);
    if ((superPieceAttacks & enemyBb) > 0) {
        return true;
    }
    enemyBb = color == Color::WHITE ? board.getBlackKnights() : board.getWhiteKnights();
    superPieceAttacks = getKnightAttacksForSquare(bitboardPosition);
    if ((superPieceAttacks & enemyBb) > 0) {
        return true;
    }
    enemyBb = color == Color::WHITE ? board.getBlackRooks() : board.getWhiteRooks();
    superPieceAttacks = getRookAttacksForSquare(board, bitboardPosition);
    if ((superPieceAttacks & enemyBb) > 0) {
        return true;
    }
    enemyBb = color == Color::WHITE ? board.getBlackBishops() : board.getWhiteBishops();
    superPieceAttacks = getBishopAttacksForSquare(board, bitboardPosition);
    if ((superPieceAttacks & enemyBb) > 0) {
        return true;
    }
    enemyBb = color == Color::WHITE ? board.getBlackQueens() : board.getWhiteQueens();
    superPieceAttacks = getQueenAttacksForSquare(board, bitboardPosition);
    if ((superPieceAttacks & enemyBb) > 0) {
        return true;
    }
    enemyBb = color == Color::WHITE ? board.getBlackKings() : board.getWhiteKings();
    superPieceAttacks = getKingAttacksForSquare(board, bitboardPosition);
    if ((superPieceAttacks & enemyBb) > 0) {
        return true;
    }

    return false;
}

void MoveGenerator::orderMoves(Board& board, std::vector<Move>& moves) {
    for (Move& move : moves) {
        //DebugUtils::prettyPrintMove(move);
        move.moveScore = evaluator.evaluateMove(board, move);
        //DebugUtils::debugPrint("Score: " + std::to_string(move.moveScore));
    }
    std::nth_element(moves.begin(), moves.begin(), moves.end(),
                 [board](const Move& a, const Move& b) {
                     if (a.isCapture != b.isCapture) {
                         return a.isCapture > b.isCapture;
                     }

                     if (board.whoseTurn == Color::WHITE) {
                         return a.moveScore > b.moveScore;
                     }
                     return a.moveScore < b.moveScore;
                 });
}


void MoveGenerator::generateLegalMoves(std::vector<Move>& outMoves, Board& board, const bool onlyCaptures) {
    std::vector<Move> tempMoves;
    generatePseudoLegalMoves(tempMoves, board);
    outMoves = RuleChecker::filterIllegalMoves(board, tempMoves);
    if (onlyCaptures) {
        std::erase_if(outMoves,
                      [](const Move& m) {
                          return !m.isCapture;
                      });
    }
    orderMoves(board, outMoves);
}

void MoveGenerator::generatePseudoLegalMoves(std::vector<Move>& moves, Board& board) {
    // For future note, should probably make this append capturing moves first, because mini-max gets better that way
    vector<Move> pawnMoves = generatePawnMoves(board);
    vector<Move> knightMoves = generateKnightMoves(board);
    vector<Move> bishopMoves = generateBishopMoves(board);
    vector<Move> rookMoves = generateRookMoves(board);
    vector<Move> queenMoves = generateQueenMoves(board);
    vector<Move> kingMoves = generateKingMoves(board);
    moves.insert(moves.end(), pawnMoves.begin(), pawnMoves.end());
    moves.insert(moves.end(), knightMoves.begin(), knightMoves.end());
    moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());
    moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());
    moves.insert(moves.end(), queenMoves.begin(), queenMoves.end());
    moves.insert(moves.end(), kingMoves.begin(), kingMoves.end());
}

vector<Move> MoveGenerator::generatePawnMoves(const Board& board) {
    vector<Move> moves = vector<Move>();

    uint64_t startRankMask;
    uint64_t finalRankMask;

    uint64_t pawnBbCopy;
    uint64_t enemyPiecesBb;
    uint64_t allPiecesBb = board.getAllPieces();
    int colorMultiplier;
    Piece pieceToMove;

    // copy all pawns bitboard
    if (board.whoseTurn == Color::WHITE) {
        pawnBbCopy = board.getWhitePawns();
        enemyPiecesBb = board.getBlackPieces();
        colorMultiplier = 1;

        pieceToMove = Piece::WHITE_PAWN;

        startRankMask = 0xFF00;
        finalRankMask = 0xFF00000000000000;
    } else {
        pawnBbCopy = board.getBlackPawns();
        enemyPiecesBb = board.getWhitePieces();
        colorMultiplier = -1;

        pieceToMove = Piece::BLACK_PAWN;

        startRankMask = 0x00FF000000000000;
        finalRankMask = 0xFF;
    }
    uint64_t lsb = Board::getLeastSignificantBit(pawnBbCopy);
    uint64_t endSquare;

    while (lsb > 0) {
        // Single advance
        bool pathClear = (allPiecesBb & shift(lsb, 8 * colorMultiplier)) == 0;
        bool startRank = (lsb & startRankMask) > 0;
        if (pathClear) {
            endSquare = shift(lsb, 8 * colorMultiplier);
            if ((endSquare & finalRankMask) > 0) {
                vector<Move> promotions = getPawnPromotions(board, lsb, endSquare);
                for (Move& move : promotions) {
                    addMove(board, moves, move);
                }
            } else {
                addMove(board, moves, Move(lsb, endSquare, pieceToMove));
            }
        }

        // Double advance
        pathClear = (allPiecesBb & shift(lsb, 8 * colorMultiplier)) == 0 && (allPiecesBb & shift(lsb, 16 * colorMultiplier)) == 0;
        if (pathClear && startRank) {
            endSquare = shift(lsb, 16 * colorMultiplier);
            auto move = Move(lsb, endSquare, pieceToMove);
            move.isEnPassantable = true;
            addMove(board, moves, move);
        }

        uint64_t A_FILE_MASK = 0x0101010101010101ULL;
        uint64_t H_FILE_MASK = 0x8080808080808080ULL;

        uint64_t leftWrapMask = board.whoseTurn == Color::WHITE ? H_FILE_MASK : A_FILE_MASK;
        uint64_t rightWrapMask = board.whoseTurn == Color::WHITE ? A_FILE_MASK : H_FILE_MASK;
        // Left capture
        endSquare = shift(lsb, 7 * colorMultiplier);
        bool isTarget = (enemyPiecesBb & endSquare & ~leftWrapMask) > 0;
        if (isTarget) {
            if ((endSquare & finalRankMask) > 0) {
                vector<Move> promotions = getPawnPromotions(board, lsb, endSquare);
                for (Move& move : promotions) {
                    addMove(board, moves, move);
                }
            } else {

                addMove(board, moves, Move(lsb, endSquare, pieceToMove));
            }

        }

        // Right capture

        endSquare = shift(lsb, 9 * colorMultiplier);

        isTarget = (enemyPiecesBb & endSquare & ~rightWrapMask) > 0;
        if (isTarget) {
            if ((endSquare & finalRankMask) > 0) {
                vector<Move> promotions = getPawnPromotions(board, lsb, endSquare);
                for (Move& move : promotions) {
                    addMove(board, moves, move);
                }
            } else {
                moves.emplace_back(lsb, endSquare, pieceToMove);
            }
        }

        // Left En-passant
        endSquare = shift(lsb, 7 * colorMultiplier);
        isTarget = (board.enPassantSquare & endSquare & ~leftWrapMask) > 0;
        if (isTarget && (allPiecesBb & endSquare) == 0) {
            auto move = Move(lsb, endSquare, pieceToMove);
            move.enPassant = true;
            addMove(board, moves, move);
        }

        // Right En-passant
        endSquare = shift(lsb, 9 * colorMultiplier);
        isTarget = (board.enPassantSquare & endSquare & ~rightWrapMask) > 0;
        if (isTarget && (allPiecesBb & endSquare) == 0) {
            auto move = Move(lsb, endSquare, pieceToMove);
            move.enPassant = true;
            addMove(board, moves, move);
        }

        // Clear LSB
        pawnBbCopy = pawnBbCopy ^ lsb;
        lsb = Board::getLeastSignificantBit(pawnBbCopy);
    }
    return moves;

}
uint64_t MoveGenerator::getPawnAttacksForSquare(Board& board, const uint64_t square, const Color color) {
    uint64_t A_FILE_MASK = 0x0101010101010101ULL;
    uint64_t H_FILE_MASK = 0x8080808080808080ULL;
    uint64_t notLeftFileMask = color == Color::WHITE ? ~A_FILE_MASK : ~H_FILE_MASK;
    uint64_t notRightFileMask = color == Color::WHITE ? ~H_FILE_MASK : ~A_FILE_MASK;
    int colorMultiplier = color == Color::WHITE ? 1 : -1;
    uint64_t attacks = 0;

    // Left Capture
    attacks |= shift(square, 7 * colorMultiplier) & notRightFileMask;

    // Right Capture
    attacks |= shift(square, 9 * colorMultiplier) & notLeftFileMask;
    return attacks;
}

vector<Move> MoveGenerator::generateKnightMoves(const Board& board) {
    vector<Move> moves = vector<Move>();
    uint64_t knightBbCopy;
    uint64_t myPiecesBb;

    Piece knight;

    if (board.whoseTurn == Color::WHITE) {
        knightBbCopy = board.getWhiteKnights();
        myPiecesBb = board.getWhitePieces();
        knight = Piece::WHITE_KNIGHT;
    } else {
        knightBbCopy = board.getBlackKnights();
        myPiecesBb = board.getBlackPieces();
        knight = Piece::BLACK_KNIGHT;
    }

    uint64_t lsb = Board::getLeastSignificantBit(knightBbCopy);

    while (lsb > 0) {

        uint64_t validMoves = getKnightAttacksForSquare(lsb);
        validMoves = validMoves & ~myPiecesBb;
        uint64_t validMovesLsb = Board::getLeastSignificantBit(validMoves);
        while (validMovesLsb > 0) {
            addMove(board, moves, Move(lsb, validMovesLsb, knight));
            validMoves = validMoves ^ validMovesLsb;
            validMovesLsb = Board::getLeastSignificantBit(validMoves);
        }

        knightBbCopy = knightBbCopy ^ lsb;
        lsb = Board::getLeastSignificantBit(knightBbCopy);
    }

    return moves;
}
uint64_t MoveGenerator::getKnightAttacksForSquare(const uint64_t bitboardPosition) {
    const uint64_t square = __builtin_ctzll(bitboardPosition);
    return preComputedKnightMoves[square];
}

vector<Move> MoveGenerator::generateBishopMoves(Board& board) {
    vector<Move> moves = vector<Move>();
    uint64_t bishopBbCopy;
    uint64_t myPiecesBb;

    Piece bishop;

    if (board.whoseTurn == Color::WHITE) {
        bishopBbCopy = board.getWhiteBishops();
        myPiecesBb = board.getWhitePieces();
        bishop = Piece::WHITE_BISHOP;
    } else {
        bishopBbCopy = board.getBlackBishops();
        myPiecesBb = board.getBlackPieces();
        bishop = Piece::BLACK_BISHOP;
    }

    uint64_t lsb = Board::getLeastSignificantBit(bishopBbCopy);


    while (lsb > 0) {

        uint64_t attacks = getBishopAttacksForSquare(board, lsb);

        // Attacks includes attacks on friendly pieces, need to mask them out
        uint64_t validMoves = attacks & ~myPiecesBb;
        uint64_t validMovesLsb = Board::getLeastSignificantBit(validMoves);

        while (validMovesLsb > 0) {
            addMove(board, moves, Move(lsb, validMovesLsb, bishop));
            validMoves = validMoves ^ validMovesLsb;
            validMovesLsb = Board::getLeastSignificantBit(validMoves);
        }

        bishopBbCopy = bishopBbCopy ^ lsb;
        lsb = Board::getLeastSignificantBit(bishopBbCopy);
    }

    return moves;
}
/**
 *
 * @param board The current state of the board
 * @param bishopSquare The bitboard where the position of the piece is 1, and every other bit is 0
 * @return The bitboard where each 1 is a valid attack
 */
uint64_t MoveGenerator::getBishopAttacksForSquare(const Board& board, const uint64_t bishopSquare) {
    const int square = __builtin_ctzll(bishopSquare);
    const uint64_t blockers = board.getAllPieces();
    const int index = static_cast<int>(((blockers & BISHOP_MASKS[square]) * BISHOP_MAGICS[square]) >> BISHOP_SHIFTS[square]);
    const uint64_t attacks = BISHOP_TABLE[BISHOP_OFFSETS[square] + index];
    return attacks;
}

vector<Move> MoveGenerator::generateRookMoves(Board& board) {
    vector<Move> moves = vector<Move>();
    uint64_t rookBbCopy;
    uint64_t myPiecesBb;

    Piece rook;

    if (board.whoseTurn == Color::WHITE) {
        rookBbCopy = board.getWhiteRooks();
        myPiecesBb = board.getWhitePieces();
        rook = Piece::WHITE_ROOK;
    } else {
        rookBbCopy = board.getBlackRooks();
        myPiecesBb = board.getBlackPieces();
        rook = Piece::BLACK_ROOK;
    }

    uint64_t lsb = Board::getLeastSignificantBit(rookBbCopy);


    while (lsb > 0) {

        uint64_t attacks = getRookAttacksForSquare(board, lsb);


        // Attacks includes attacks on friendly pieces, need to mask them out
        uint64_t validMoves = attacks & ~myPiecesBb;
        uint64_t validMovesLsb = Board::getLeastSignificantBit(validMoves);

        while (validMovesLsb > 0) {
            addMove(board, moves, Move(lsb, validMovesLsb, rook));
            validMoves = validMoves ^ validMovesLsb;
            validMovesLsb = Board::getLeastSignificantBit(validMoves);
        }

        rookBbCopy = rookBbCopy ^ lsb;
        lsb = Board::getLeastSignificantBit(rookBbCopy);
    }

    return moves;
}
/**
 *
 * @param board The current state of the board
 * @param rookSquare The bitboard where the position of the piece is 1, and every other bit is 0
 * @return The bitboard where each 1 is a valid attack
 */
uint64_t MoveGenerator::getRookAttacksForSquare(const Board& board, const uint64_t rookSquare) {
    const int square = __builtin_ctzll(rookSquare);
    const uint64_t blockers = board.getAllPieces();
    const int index = static_cast<int>(((blockers & ROOK_MASKS[square]) * ROOK_MAGICS[square]) >> ROOK_SHIFTS[square]);
    const uint64_t attacks = ROOK_TABLE[ROOK_OFFSETS[square] + index];
    return attacks;
}

vector<Move> MoveGenerator::generateQueenMoves(Board& board) {
    vector<Move> moves = vector<Move>();
    uint64_t queenBbCopy;
    uint64_t myPiecesBb;

    Piece queen;

    if (board.whoseTurn == Color::WHITE) {
        queenBbCopy = board.getWhiteQueens();
        myPiecesBb = board.getWhitePieces();
        queen = Piece::WHITE_QUEEN;
    } else {
        queenBbCopy = board.getBlackQueens();
        myPiecesBb = board.getBlackPieces();
        queen = Piece::BLACK_QUEEN;
    }

    uint64_t lsb = Board::getLeastSignificantBit(queenBbCopy);


    while (lsb > 0) {

        const uint64_t attacks = getQueenAttacksForSquare(board, lsb);


        // Attacks includes attacks on friendly pieces, need to mask them out
        uint64_t validMoves = attacks & ~myPiecesBb;
        uint64_t validMovesLsb = Board::getLeastSignificantBit(validMoves);

        while (validMovesLsb > 0) {
            addMove(board, moves, Move(lsb, validMovesLsb, queen));
            validMoves = validMoves ^ validMovesLsb;
            validMovesLsb = Board::getLeastSignificantBit(validMoves);
        }

        queenBbCopy = queenBbCopy ^ lsb;
        lsb = Board::getLeastSignificantBit(queenBbCopy);
    }

    return moves;
}
uint64_t MoveGenerator::getQueenAttacksForSquare(const Board& board, const uint64_t square) {
    return getRookAttacksForSquare(board, square) | getBishopAttacksForSquare(board, square);
}

vector<Move> MoveGenerator::generateKingMoves(Board& board) {
    vector<Move> moves = vector<Move>();
    uint64_t kingSquare;

    uint64_t myPiecesBb;
    uint64_t myRooksBb;
    uint64_t allPiecesBb = board.getAllPieces();


    uint64_t startSquareMask;

    Piece king;

    bool hasShortCastlingRights;
    bool hasLongCastlingRights;

    if (board.whoseTurn == Color::WHITE) {
        kingSquare = board.getWhiteKings();
        myPiecesBb = board.getWhitePieces();
        myRooksBb = board.getWhiteRooks();
        king = Piece::WHITE_KING;
        startSquareMask = 0x10;

        hasShortCastlingRights = board.castlingRights >> 2 & 1;
        hasLongCastlingRights = board.castlingRights >> 3 & 1;

    } else {
        kingSquare = board.getBlackKings();
        myPiecesBb = board.getBlackPieces();
        myRooksBb = board.getBlackRooks();
        king = Piece::BLACK_KING;
        startSquareMask = 0x1000000000000000;

        hasShortCastlingRights = board.castlingRights & 1;
        hasLongCastlingRights = board.castlingRights >> 1 & 1;
    }

    uint64_t attacks = getKingAttacksForSquare(board, kingSquare);
    attacks &= ~myPiecesBb;

    uint64_t attacksLsb = Board::getLeastSignificantBit(attacks);
    while (attacksLsb > 0) {
        addMove(board, moves, Move(kingSquare, attacksLsb, king));
        attacks = attacks ^ attacksLsb;
        attacksLsb = Board::getLeastSignificantBit(attacks);
    }


    bool onStartSquare = (kingSquare & startSquareMask) > 0;
    bool kingNotAttacked = !isSquareAttacked(board, kingSquare, board.whoseTurn);

    // Short Castling
    bool shortCastlePathClear = (kingSquare << 1 & allPiecesBb) == 0 && (kingSquare << 2 & allPiecesBb) == 0;
    bool shortCastlePathNotAttacked = !isSquareAttacked(board, kingSquare << 1, board.whoseTurn)
        && !isSquareAttacked(board, kingSquare << 2, board.whoseTurn);
    bool shortCastleRook = (kingSquare << 3 & myRooksBb) > 0;
    bool canShortCastle = onStartSquare && kingNotAttacked && shortCastlePathClear && shortCastleRook && hasShortCastlingRights && shortCastlePathNotAttacked;
    if (canShortCastle) {
        Move shortCastle = Move(kingSquare, kingSquare << 2, king);
        shortCastle.isShortCastling = true;
        addMove(board, moves, shortCastle);
    }

    // Long Castling
    bool longCastlePathClear = (kingSquare >> 1 & allPiecesBb) == 0 && (kingSquare >> 2 & allPiecesBb) == 0 && (kingSquare >> 3 & allPiecesBb) == 0;
    bool longCastlePathNotAttacked = !isSquareAttacked(board, kingSquare >> 1, board.whoseTurn)
        && !isSquareAttacked(board, kingSquare >> 2, board.whoseTurn)
        && !isSquareAttacked(board, kingSquare >> 3, board.whoseTurn);
    bool longCastleRook = (kingSquare >> 4 & myRooksBb) > 0;
    bool canLongCastle = onStartSquare && kingNotAttacked && longCastlePathClear && longCastleRook && hasLongCastlingRights && longCastlePathNotAttacked;
    if (canLongCastle) {
        Move longCastle = Move(kingSquare, kingSquare >> 2, king);
        longCastle.isLongCastling = true;
        addMove(board, moves, longCastle);
    }


    return moves;
}
uint64_t MoveGenerator::getKingAttacksForSquare(Board& board, uint64_t square) {
    uint64_t attacks = 0;

    uint64_t A_FILE_MASK = 0x0101010101010101ULL;
    uint64_t H_FILE_MASK = 0x8080808080808080ULL;

    // East
    attacks |= (square & ~H_FILE_MASK) << 1ULL;
    // West
    attacks |= (square & ~A_FILE_MASK) >> 1ULL;
    // North-West
    attacks |= (square & ~A_FILE_MASK) << 7ULL;
    // North
    attacks |= square << 8ULL;
    // North-East
    attacks |= (square & ~H_FILE_MASK) << 9ULL;
    // South-West
    attacks |= (square & ~A_FILE_MASK) >> 9ULL;
    // South
    attacks |= square >> 8ULL;
    // South-East
    attacks |= (square & ~H_FILE_MASK) >> 7ULL;
    return attacks;
}



vector<Move> MoveGenerator::getPawnPromotions(const Board& board, const uint64_t startSquare, const uint64_t endSquare) {
    Color myColor = board.whoseTurn;
    vector<Move> moves;
    Piece pawn;
    Piece knight;
    Piece bishop;
    Piece rook;
    Piece queen;
    if (myColor == Color::WHITE) {
        pawn = Piece::WHITE_PAWN;
        knight = Piece::WHITE_KNIGHT;
        bishop = Piece::WHITE_BISHOP;
        rook = Piece::WHITE_ROOK;
        queen = Piece::WHITE_QUEEN;
    } else {
        pawn = Piece::BLACK_PAWN;
        knight = Piece::BLACK_KNIGHT;
        bishop = Piece::BLACK_BISHOP;
        rook = Piece::BLACK_ROOK;
        queen = Piece::BLACK_QUEEN;
    }
    Move move = Move(startSquare, endSquare, pawn);
    move.isPromotion = true;
    move.promotionType = knight;
    moves.push_back(move);

    move = Move(startSquare, endSquare, pawn);
    move.isPromotion = true;
    move.promotionType = bishop;
    moves.push_back(move);

    move = Move(startSquare, endSquare, pawn);
    move.isPromotion = true;
    move.promotionType = rook;
    moves.push_back(move);

    move = Move(startSquare, endSquare, pawn);
    move.isPromotion = true;
    move.promotionType = queen;
    moves.push_back(move);

    return moves;
}

void MoveGenerator::addMove(const Board& board, std::vector<Move>& moves, Move move) {
    if (board.getPieceAtSquare(move.getEndSquare()) != Piece::EMPTY) {
        move.isCapture = true;
    }
    moves.push_back(move);
}
