//
// Created by Mercury on 23/02/2026.
//

#ifndef CHESS_ENGINE_RULECHECKER_H
#define CHESS_ENGINE_RULECHECKER_H
#include "Board.h"
#include "Move.h"
#include "MoveGenerator.h"


class RuleChecker {
    public:
        [[nodiscard]] static std::vector<Move> filterIllegalMoves(Board& board, const std::vector<Move>& pseudoLegalMoves);
        static bool isColorInCheck(Board& board, Color color);
        static bool isCheckmate(Board& board);
    private:
        static bool isMoveLegal(Board& board, const Move& move);
        static bool isDraw();
        static bool isLoss();

};


#endif //CHESS_ENGINE_RULECHECKER_H