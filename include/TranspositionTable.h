//
// Created by Mercury on 03/07/2026.
//

#ifndef CHESS_ENGINE_TRANSPOSITIONTABLE_H
#define CHESS_ENGINE_TRANSPOSITIONTABLE_H
#include <cstdint>
#include <string>
#include <vector>

#include "Move.h"


class TranspositionTable {
    public:
        explicit TranspositionTable(size_t sizeMB);
        enum class Flag {
            EXACT,
            LOWERBOUND,
            UPPERBOUND,
        };
        struct TTEntry {
            uint64_t key = 0;
            int depth = -1;
            int score = 0;
            Move bestMove = Move(0, 0, Piece::WHITE_PAWN);
            Flag flag = Flag::EXACT;
            bool valid = false;
        };
        void store(uint64_t key, int depth, int score, Move bestMove, Flag flag);
        TTEntry search(uint64_t key) const;
    private:
        std::vector<TTEntry> table;
        size_t tableSize;
};


#endif //CHESS_ENGINE_TRANSPOSITIONTABLE_H