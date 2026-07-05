//
// Created by Mercury on 23/02/2026.
//

#include "../include/Evaluator.h"


#include "../include/BoardManager.h"
#include "../include/DebugUtils.h"
#include "../include/MoveGenerator.h"
#include "../include/RuleChecker.h"

int lerpInt(int start, int end, float t) {
    return static_cast<int>(start + t * (end - start));
}

float computeEndgameWeight(int currentTurn, int startTurn, int endTurn) {
    if (currentTurn <= startTurn)
        return 0.0f;  // fully early game
    if (currentTurn >= endTurn)
        return 1.0f;  // fully endgame

    // linear interpolation between startTurn and fullEndgameTurn
    return float(currentTurn - startTurn) / float(endTurn - startTurn);
}

Evaluator::Evaluator() = default;

int Evaluator::evaluateBoard(Board& board) {
    int whiteScore = 0;
    int blackScore = 0;
    int finalScore = 0;

    // White pieces add positive score
    whiteScore += materialValue(board, Color::WHITE);
    whiteScore += positionValue(board, Color::WHITE);

    // Black pieces subtract score
    blackScore += materialValue(board, Color::BLACK);
    blackScore += positionValue(board, Color::BLACK);

    finalScore = whiteScore - blackScore;
    int perspective = board.whoseTurn == Color::WHITE ? 1 : -1;
    finalScore *= perspective;
    return finalScore;
}

int Evaluator::evaluateMove(Board& board, const Move& move) {
    int score = 0;
    Piece movingPiece = move.getPiece();
    Piece capturedPiece = board.getPieceAtSquare(move.getEndSquare());

    if (capturedPiece != Piece::EMPTY) {
        score += 10 * getValueOfPiece(capturedPiece) - getValueOfPiece(movingPiece);
    }

    if (move.isPromotion) {
        score += getValueOfPiece(move.promotionType);
    }

    uint64_t enemyPawns = board.whoseTurn == Color::WHITE ? board.getBlackPawns() : board.getWhitePawns();
    if (MoveGenerator::getPawnAttacksForSquare(board, move.getEndSquare(), board.whoseTurn) & enemyPawns) {
        score -= getValueOfPiece(movingPiece);
    }

    return score;
}

int Evaluator::getValueOfPiece(const Piece piece) const {

    switch (piece) {
        case Piece::WHITE_PAWN:
        case Piece::BLACK_PAWN:
            return pawnValue;

        case Piece::WHITE_KNIGHT:
        case Piece::BLACK_KNIGHT:
            return knightValue;

        case Piece::WHITE_BISHOP:
        case Piece::BLACK_BISHOP:
            return bishopValue;

        case Piece::WHITE_ROOK:
        case Piece::BLACK_ROOK:
            return rookValue;

        case Piece::WHITE_QUEEN:
        case Piece::BLACK_QUEEN:
            return queenValue;

        default:
            return 0;
    }
}


int Evaluator::materialValue(const Board& board, const Color color) const {
    int whiteValue = 0;
    int blackValue = 0;
    whiteValue += __builtin_popcountll(board.getWhitePawns()) * pawnValue;
    blackValue += __builtin_popcountll(board.getBlackPawns()) * pawnValue;

    whiteValue += __builtin_popcountll(board.getWhiteKnights()) * knightValue;
    blackValue += __builtin_popcountll(board.getBlackKnights()) * knightValue;

    whiteValue += __builtin_popcountll(board.getWhiteBishops()) * bishopValue;
    blackValue += __builtin_popcountll(board.getBlackBishops()) * bishopValue;

    whiteValue += __builtin_popcountll(board.getWhiteRooks()) * rookValue;
    blackValue += __builtin_popcountll(board.getBlackRooks()) * rookValue;

    whiteValue += __builtin_popcountll(board.getWhiteQueens()) * queenValue;
    blackValue += __builtin_popcountll(board.getBlackQueens()) * queenValue;

    if (color == Color::WHITE) {
        return whiteValue;
    }

    return blackValue;
}

int Evaluator::positionValue(const Board& board, const Color color) const {
    int score = 0;
    uint64_t lsb;
    if (color == Color::WHITE) {
        uint64_t pawnBb = board.getWhitePawns();
        lsb = Board::getLeastSignificantBit(pawnBb);
        while (lsb != 0) {
            int squareNum = __builtin_ctzll(lsb);
            score += whitePawnTable[squareNum];
            pawnBb = pawnBb ^ lsb;
            lsb = Board::getLeastSignificantBit(pawnBb);
        }

        uint64_t knightBb = board.getWhiteKnights();
        lsb = Board::getLeastSignificantBit(knightBb);
        while (lsb != 0) {
            int squareNum = __builtin_ctzll(lsb);
            score += whiteKnightTable[squareNum];
            knightBb = knightBb ^ lsb;
            lsb = Board::getLeastSignificantBit(knightBb);
        }

        uint64_t bishopBb = board.getWhiteBishops();
        lsb = Board::getLeastSignificantBit(bishopBb);
        while (lsb != 0) {
            int squareNum = __builtin_ctzll(lsb);
            score += whiteBishopTable[squareNum];
            bishopBb = bishopBb ^ lsb;
            lsb = Board::getLeastSignificantBit(bishopBb);
        }
        uint64_t rookBb = board.getWhiteRooks();
        lsb = Board::getLeastSignificantBit(rookBb);
        while (lsb != 0) {
            int squareNum = __builtin_ctzll(lsb);
            score += whiteRookTable[squareNum];
            rookBb = rookBb ^ lsb;
            lsb = Board::getLeastSignificantBit(rookBb);
        }

        uint64_t queenBb = board.getWhiteQueens();
        lsb = Board::getLeastSignificantBit(queenBb);
        while (lsb != 0) {
            int squareNum = __builtin_ctzll(lsb);
            score += whiteQueenTable[squareNum];
            queenBb = queenBb ^ lsb;
            lsb = Board::getLeastSignificantBit(queenBb);
        }

        uint64_t kingBb = board.getWhiteKings();
        lsb = Board::getLeastSignificantBit(kingBb);
        while (lsb != 0) {
            int squareNum = __builtin_ctzll(lsb);
            float endGameWeight = computeEndgameWeight(BoardManager::fullMoveClock, ENDGAME_MOVE_STARTWEIGHT, ENDGAME_MOVE_FULLWEIGHT);
            int value = lerpInt(whiteKingTableEarlyGame[squareNum], whiteKingTableEndGame[squareNum], endGameWeight);
            score += value;
            kingBb = kingBb ^ lsb;
            lsb = Board::getLeastSignificantBit(kingBb);
        }

    } else {
        uint64_t pawnBb = board.getBlackPawns();
        lsb = Board::getLeastSignificantBit(pawnBb);
        while (lsb != 0) {
            int squareNum = __builtin_ctzll(lsb);
            score += blackPawnTable[squareNum];
            pawnBb = pawnBb ^ lsb;
            lsb = Board::getLeastSignificantBit(pawnBb);
        }

        uint64_t knightBb = board.getBlackKnights();
        lsb = Board::getLeastSignificantBit(knightBb);
        while (lsb != 0) {
            int squareNum = __builtin_ctzll(lsb);
            score += blackKnightTable[squareNum];
            knightBb = knightBb ^ lsb;
            lsb = Board::getLeastSignificantBit(knightBb);
        }

        uint64_t bishopBb = board.getBlackBishops();
        lsb = Board::getLeastSignificantBit(bishopBb);
        while (lsb != 0) {
            int squareNum = __builtin_ctzll(lsb);
            score += blackBishopTable[squareNum];
            bishopBb = bishopBb ^ lsb;
            lsb = Board::getLeastSignificantBit(bishopBb);
        }

        uint64_t rookBb = board.getWhiteRooks();
        lsb = Board::getLeastSignificantBit(rookBb);
        while (lsb != 0) {
            int squareNum = __builtin_ctzll(lsb);
            score += blackRookTable[squareNum];
            rookBb = rookBb ^ lsb;
            lsb = Board::getLeastSignificantBit(rookBb);
        }

        uint64_t queenBb = board.getWhiteQueens();
        lsb = Board::getLeastSignificantBit(queenBb);
        while (lsb != 0) {
            int squareNum = __builtin_ctzll(lsb);
            score += blackQueenTable[squareNum];
            queenBb = queenBb ^ lsb;
            lsb = Board::getLeastSignificantBit(queenBb);
        }

        uint64_t kingBb = board.getBlackKings();
        lsb = Board::getLeastSignificantBit(kingBb);
        while (lsb != 0) {
            int squareNum = __builtin_ctzll(lsb);
            float endGameWeight = computeEndgameWeight(BoardManager::fullMoveClock, ENDGAME_MOVE_STARTWEIGHT, ENDGAME_MOVE_FULLWEIGHT);
            int value = lerpInt(blackKingTableEarlyGame[squareNum], blackKingTableEndGame[squareNum], endGameWeight);
            score += value;
            kingBb = kingBb ^ lsb;
            lsb = Board::getLeastSignificantBit(kingBb);}
    }
    return score;
}


