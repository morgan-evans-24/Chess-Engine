//
// Created by Mercury on 24/02/2026.
//

#ifndef CHESS_ENGINE_BITBOARDPRINTER_H
#define CHESS_ENGINE_BITBOARDPRINTER_H
#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

#include "Board.h"
#include "Move.h"

using std::chrono::time_point, std::chrono::steady_clock;

class DebugUtils {
    public:
        static void printBitboard(uint64_t bitboard);
        static void printBoardState(const Board& board);
        static void prettyPrintMoveList(const std::vector<Move>& moves);
        static void prettyPrintMove(const Move& move);
        static std::string moveToString(const Move& move);
        static std::string squareToString(uint64_t square);
        static void debugPrint(std::string s);
        static void startMoveTimer();
        static void printMoveTime();
        static void printNumExploredPositions();
        static constexpr bool debugMode = true;
        static int numExploredPositions;
        static int numExploredQuiescencePositions;

    private:
        static time_point<steady_clock> timeBeforeMove;
        static char pieceToChar(Piece piece);
};


#endif //CHESS_ENGINE_BITBOARDPRINTER_H