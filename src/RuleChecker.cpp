//
// Created by Mercury on 23/02/2026.
//

#include "../include/RuleChecker.h"

std::vector<Move> RuleChecker::filterIllegalMoves(Board& board, const std::vector<Move>& pseudoLegalMoves) {
    std::vector<Move> legalMoves;
    legalMoves.reserve(pseudoLegalMoves.size());

    for (const Move& move : pseudoLegalMoves) {
        if (isMoveLegal(board, move)) {
            legalMoves.push_back(move);
        }
    }
    return legalMoves;
}

bool RuleChecker::isMoveLegal(Board& board, const Move& move) {
    const Color myColor = board.whoseTurn;
    board.makeMove(move);
    const bool isLegal = !isColorInCheck(board, myColor);
    board.unmakeMove(move);
    return isLegal;
}

bool RuleChecker::isColorInCheck(Board& board, const Color color) {
    uint64_t kingSquare = color == Color::WHITE ? board.getWhiteKings() : board.getBlackKings();
    if (MoveGenerator::isSquareAttacked(board, kingSquare, color)) {
        return true;
    }
    return false;
}

bool RuleChecker::isCheckmate(Board& board) {
    Color myColor = board.whoseTurn;
    if (isColorInCheck(board, myColor)) {
        std::vector<Move> moves;
        // MoveGenerator::generateLegalMoves(moves, board, false);
        for (const Move& move : moves) {
            board.makeMove(move);
            if (!isColorInCheck(board, myColor)) {
                return false;
            }
            board.unmakeMove(move);
        }
        return true;
    }
    return false;
}

